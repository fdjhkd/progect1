#include "GameRequestHandler.h"

/*
 Constructor for GameRequestHandler.
 param: gameManager - reference to GameManager
 param: gameId - ID of the game to handle
 param: user - currently logged user
 param: handlerFactory - factory for request handlers
 time: O(log n) for game lookup
*/
GameRequestHandler::GameRequestHandler(GameManager& gameManager, unsigned int gameId, const LoggedUser& user, RequestHandlerFactory* handlerFactory)
	: m_gameManager(gameManager), m_user(user), m_handlerFactory(handlerFactory), m_game(m_gameManager.getGameById(gameId)), m_gotQuestion(false)
{
}

/*
 Checks if the type field in JSON matches one of the accepted commands.
 param: jsonData - parsed JSON object
 return: true if type is valid, false otherwise
 time: O(1)
*/
bool GameRequestHandler::isTypeValid(const nlohmann::json& jsonData) const
{
	if (!jsonData.contains(REQUEST_TYPE))
		return false;

	std::string type = jsonData[REQUEST_TYPE];
	return type == SUBMIT_ANSWER || type == GET_GAME_RESULTS || type == GET_QUESTION || type == BACK_TO_MENU;
}

/*
 Checks if the request is relevant.
 param: req - incoming request
 return: true if request type is valid JSON and recognized
 time: O(n)
*/
bool GameRequestHandler::isRequestRelevant(const Request& req)
{
	try
	{
		nlohmann::json jsonData = nlohmann::json::parse(req.buffer);
		return isTypeValid(jsonData);
	}
	catch (const std::exception&)
	{
		return false;
	}
}

/*
 Handles the get_question request logic.
 param: result - pointer to RequestResult to modify or replace
 param: req - original request
 time: O(1)
*/
void GameRequestHandler::handleGetQuestion(RequestResult*& result, const Request& req)
{
	if (m_game.userFinished(m_user))
	{
		result->response = ERR_NO_QUESTIONS_LEFT;
		return;
	}

	if (!m_gotQuestion)
	{
		m_timeToAnswer = req.recivalTime;
		m_gotQuestion = true;

		delete result;
		GetQuestionResponse* gqr = getQuestion(req);
		gqr->response = RESPONSE_PREFIX_GET_QUESTION + gqr->response;
		result = gqr;
	}
	else
	{
		result->response = ERR_ALREADY_GOT_QUESTION;
	}
}

/*
 Handles the get_game_results request logic.
 param: result - pointer to RequestResult to modify or replace
 param: req - original request
 time: O(1)
*/
void GameRequestHandler::handleGetGameResults(RequestResult*& result, const Request& req)
{
	delete result;
	GetGameResultsResponse* ggrr = getGameResults(req);
	ggrr->response = RESPONSE_PREFIX_GET_GAME_RESULTS + ggrr->response;
	result = ggrr;
}

/*
 Handles the submit_answer request logic.
 param: result - pointer to RequestResult to modify or replace
 param: req - original request
 time: O(1)
*/
void GameRequestHandler::handleSubmitAnswer(RequestResult*& result, const Request& req)
{
	if (m_game.isGameFinished())
	{
		result->response = ERR_ALL_ANSWERED;

		if (!m_game.addedToDB())
		{
			m_gameManager.addGameToDB(m_game);
			m_game.addToDB();
		}
		return;
	}

	if (m_gotQuestion)
	{
		m_timeToAnswer = req.recivalTime - m_timeToAnswer;
		m_gotQuestion = false;

		delete result;
		SubmitAnswerResponse* sar = submitAnswer(req);
		sar->response = RESPONSE_PREFIX_SUBMIT_ANSWER + sar->response;
		result = sar;
	}
	else
	{
		result->response = ERR_DIDNT_GET_QUESTION;
	}
}

/*
 Handles the back_to_menu request logic.
 param: result - pointer to RequestResult to modify
 time: O(1)
*/
void GameRequestHandler::handleBackToMenu(RequestResult*& result)
{
	result->response = std::string(RESPONSE_PREFIX_BACK_TO_MENU) + "success";
	result->username = m_user.getUsername();
	result->requestHandler = this;

	m_gameManager.updateSingleUserStats(m_user, m_game);

	if (m_game.getPlayers().size() <= 1)
	{
		m_gameManager.addGameToDB(m_game);
		m_game.addToDB();
		m_gameManager.deleteGame(m_game);
	}
	else
	{
		m_game.removePlayer(m_user);
	}
}

/*
 Main dispatcher for incoming requests.
 param: req - incoming request
 return: pointer to response object
 time: O(n) where n depends on internal logic
*/
RequestResult* GameRequestHandler::handleRequest(const Request& req)
{
	RequestResult* result = new RequestResult();
	result->response = RESPONSE_INVALID;
	result->requestHandler = nullptr;
	result->username = m_user.getUsername();

	if (!isRequestRelevant(req))
		return result;

	nlohmann::json jsonData = nlohmann::json::parse(req.buffer);
	std::string type = jsonData[REQUEST_TYPE];

	if (type == GET_QUESTION)
		handleGetQuestion(result, req);
	else if (type == GET_GAME_RESULTS)
		handleGetGameResults(result, req);
	else if (type == SUBMIT_ANSWER)
		handleSubmitAnswer(result, req);
	else if (type == BACK_TO_MENU)
		handleBackToMenu(result);

	return result;
}

/*
 Creates and returns GetQuestionResponse for current user.
 param: req - original request
 return: pointer to GetQuestionResponse
 time: O(1)
*/
GetQuestionResponse* GameRequestHandler::getQuestion(const Request& req)
{
	GetQuestionResponse* gqr = new GetQuestionResponse(RESPONSE_INVALID, nullptr, m_user.getUsername(), FAIL, "", std::map<unsigned int, std::string>());
	Question question = m_game.getQuestionForUser(m_user);

	gqr->answers = question.getAnswers();
	gqr->question = question.getQuestion();
	gqr->requestHandler = this;
	gqr->status = SUCCESS;
	gqr->response = "success";

	return gqr;
}

/*
 Creates and returns GetGameResultsResponse for current game.
 param: req - original request
 return: pointer to GetGameResultsResponse
 time: O(1)
*/
GetGameResultsResponse* GameRequestHandler::getGameResults(const Request& req)
{
	GetGameResultsResponse* ggrr = new GetGameResultsResponse(RESPONSE_INVALID, nullptr, m_user.getUsername(), FAIL, std::vector<PlayerResults>());

	ggrr->requestHandler = this;
	ggrr->response = "success";
	ggrr->results = m_game.getResults();
	ggrr->status = SUCCESS;

	return ggrr;
}

/*
 Processes SubmitAnswerRequest and returns response.
 param: req - original request
 return: pointer to SubmitAnswerResponse
 time: O(1)
*/
SubmitAnswerResponse* GameRequestHandler::submitAnswer(const Request& req)
{
	SubmitAnswerResponse* ret = new SubmitAnswerResponse(RESPONSE_INVALID, nullptr, m_user.getUsername(), FAIL, 0);
	JsonPacketDeserializer deserializer;

	try
	{
		SubmitAnswerRequest sar = deserializer.deserializeSubmitAnswerRequest(req);
		unsigned int correctAnsId = m_game.submitAnswer(m_user, sar.answerId, m_timeToAnswer);
		ret->correctAnswerId = correctAnsId;
		ret->response = "success";
		ret->requestHandler = this;
		ret->status = SUCCESS;
		return ret;
	}
	catch (const std::exception& exp)
	{
		std::string resp = exp.what();
		if (resp == "Invalid answer id submitted")
			ret->response = ERR_INVALID_ID;
		else if (resp == "No answer id submitted")
			ret->response = ERR_NO_ID;
		return ret;
	}
}
