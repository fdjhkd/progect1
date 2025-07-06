#pragma once
#include "GameManager.h"
#include "IRequestHandler.h"
#include "json.hpp"
#include "JsonPacketDeserializer.h"

#define REQUEST_TYPE "type"
#define SUBMIT_ANSWER "submit_answer"
#define GET_GAME_RESULTS "get_game_results"
#define GET_QUESTION "get_question"
#define BACK_TO_MENU "back_to_menu"

#define ERR_NO_QUESTIONS_LEFT "get_question: err_no_questions_left"
#define ERR_ALREADY_GOT_QUESTION "get_question: err_already_got_question"
#define ERR_ALL_ANSWERED "submit_answer: err_all_answered"
#define ERR_DIDNT_GET_QUESTION "submit_answer: err_didnt_get_question"
#define ERR_INVALID_ID "err_invalid_id"
#define ERR_NO_ID "err_no_id"

#define RESPONSE_PREFIX_GET_QUESTION "get_question: "
#define RESPONSE_PREFIX_GET_GAME_RESULTS "get_game_results: "
#define RESPONSE_PREFIX_SUBMIT_ANSWER "submit_answer: "
#define RESPONSE_PREFIX_BACK_TO_MENU "back_to_menu: "
#define RESPONSE_INVALID "invalid"

class RequestHandlerFactory;

class GameRequestHandler : public IRequestHandler
{
	GameManager& m_gameManager;
	Game& m_game;
	LoggedUser m_user;
	RequestHandlerFactory* m_handlerFactory;
	bool m_gotQuestion;

	time_t m_timeToAnswer;

private:
	bool isTypeValid(const nlohmann::json& jsonData) const;
	void handleGetQuestion(RequestResult*& result, const Request& req);
	void handleGetGameResults(RequestResult*& result, const Request& req);
	void handleSubmitAnswer(RequestResult*& result, const Request& req);
	void handleBackToMenu(RequestResult*& result);

public:
	GameRequestHandler(GameManager& gameManager, unsigned int gameId, const LoggedUser& user, RequestHandlerFactory* handlerFactory);

	virtual bool isRequestRelevant(const Request& req) override;
	virtual RequestResult* handleRequest(const Request& req) override;

	GetQuestionResponse* getQuestion(const Request& req);
	GetGameResultsResponse* getGameResults(const Request& req);
	SubmitAnswerResponse* submitAnswer(const Request& req);
};
