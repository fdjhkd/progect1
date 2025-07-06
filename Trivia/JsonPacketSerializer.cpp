#include "JsonPacketSerializer.h"


/*
 This function builds a JSON response string from a response type and optional data.
 param: responseType - the type of the response
 param: data - optional JSON-formatted data to include
 return: std::string - complete JSON response
 time: O(n)
*/
inline std::string makeJsonResponse(const std::string& responseType, const std::string& data = "")
{
	return "{\"" RESPONSE_KEY "\": \"" + responseType + "\"" + (data.empty() ? "" : ", \"" DATA_KEY "\": " + data) + "}";
}

/*
 This function serializes an error response to a plain string (message only).
 param: response - the ErrorResponse object
 return: std::string - error message
 time: O(1)
*/
std::string JsonPacketSerializer::serializeResponse(const ErrorResponse& response)
{
	return response.message;
}

/*
 This function serializes a login response to a JSON string.
 param: response - the LoginResponse object
 return: std::string - JSON response
 time: O(1)
*/
std::string JsonPacketSerializer::serializeResponse(const LoginResponse& response)
{
	return makeJsonResponse(response.response);
}

/*
 This function serializes a signup response to a JSON string.
 param: response - the SignupResponse object
 return: std::string - JSON response
 time: O(1)
*/
std::string JsonPacketSerializer::serializeResponse(const SignupResponse& response)
{
	return makeJsonResponse(response.response);
}

/*
 This function serializes a join room response to a JSON string.
 param: response - the JoinRoomResponse object
 return: std::string - JSON response
 time: O(1)
*/
std::string JsonPacketSerializer::serializeResponse(const JoinRoomResponse& response)
{
	return makeJsonResponse(response.response);
}

/*
 This function serializes a join room response to a JSON string.
 param: response - the JoinRoomResponse object
 return: std::string - JSON response
 time: O(1)
*/
std::string JsonPacketSerializer::serializeResponse(const JoinRoomResponse& response)
{
	return makeJsonResponse(response.response);
}

/*
 This function serializes a create room response to a JSON string.
 param: response - the CreateRoomResponse object
 return: std::string - JSON response
 time: O(1)
*/
std::string JsonPacketSerializer::serializeResponse(const CreateRoomResponse& response)
{
	return makeJsonResponse(response.response);
}

/*
 This function serializes a close room response to a JSON string.
 param: response - the CloseRoomResponse object
 return: std::string - JSON response
 time: O(1)
*/
std::string JsonPacketSerializer::serializeResponse(const CloseRoomResponse& response)
{
	return makeJsonResponse(response.response);
}


/*
 This function serializes a start game response to a JSON string.
 param: response - the StartGameResponse object
 return: std::string - JSON response
 time: O(1)
*/
std::string JsonPacketSerializer::serializeResponse(const StartGameResponse& response)
{
	return makeJsonResponse(response.response);
}

/*
 This function serializes a leave room response to a JSON string.
 param: response - the LeaveRoomResponse object
 return: std::string - JSON response
 time: O(1)
*/
std::string JsonPacketSerializer::serializeResponse(const LeaveRoomResponse& response)
{
	return makeJsonResponse(response.response);
}

/*
 This function serializes the state of a room including players, game status, and timeout.
 param: response - the GetRoomStateResponse object
 return: std::string - JSON-formatted room state
 time: O(n)
*/
std::string JsonPacketSerializer::serializeResponse(const GetRoomStateResponse& response)
{
	std::string playersJson = "{";
	for (size_t i = 0; i < response.players.size(); ++i)
	{
		if (i > 0) playersJson += ", ";
		playersJson += "\"" + std::to_string(i) + "\": \"" + response.players[i] + "\"";
	}
	playersJson += "}";

	std::string data = "{\"" HAS_GAME_BEGUN_KEY "\": " + std::to_string(response.hasGameBegun) +
		", \"" QUESTION_COUNT_KEY "\": " + std::to_string(response.questionCount) +
		", \"answer_timeout\": " + std::to_string(response.answerTimeOut) +
		", \"" USERS_KEY "\": " + playersJson + "}";

	return makeJsonResponse(response.response, data);
}

/*
 This function serializes the list of available rooms with their metadata.
 param: response - the GetRoomsResponse object
 return: std::string - JSON-formatted room list
 time: O(n)
*/
std::string JsonPacketSerializer::serializeResponse(const GetRoomsResponse& response)
{
	std::string roomJson = "{";
	for (size_t i = 0; i < response.rooms.size(); ++i)
	{
		if (i > 0) roomJson += ", ";
		const RoomData& room = response.rooms[i];
		roomJson += "\"" + std::to_string(room.id) + "\": {\"name\": \"" + room.name +
			"\", \"" MAX_PLAYERS_KEY "\": " + std::to_string(room.maxPlayers) +
			", \"" QUESTION_COUNT_KEY "\": " + std::to_string(room.questionCount) +
			", \"" QUESTION_TIMEOUT_KEY "\": " + std::to_string(room.timePerQuestion) +
			", \"" IS_ACTIVE_KEY "\": " + std::to_string(room.isActive) + "}";
	}
	roomJson += "}";
	return makeJsonResponse(response.response, roomJson);
}

/*
 This function serializes a highscore table of users and their scores.
 param: response - the HighscoreResponse object
 return: std::string - JSON-formatted highscores
 time: O(n)
*/
std::string JsonPacketSerializer::serializeResponse(const HighscoreResponse& response)
{
	std::string userJson = "{";
	bool first = true;
	for (const auto& pair : response.highscores)
	{
		if (!first) userJson += ", ";
		first = false;
		userJson += "\"" + pair.first.getUsername() + "\": " + std::to_string(pair.second);
	}
	userJson += "}";
	return makeJsonResponse(response.response, userJson);
}

/*
 This function serializes a list of users currently in a room.
 param: response - the GetPlayersInRoomResponse object
 return: std::string - JSON list of users
 time: O(n)
*/
std::string JsonPacketSerializer::serializeResponse(const GetPlayersInRoomResponse& response)
{
	std::string userJson = "{";
	for (size_t i = 0; i < response.usersInRoom.size(); ++i)
	{
		if (i > 0) userJson += ", ";
		userJson += "\"" + std::to_string(i) + "\": \"" + response.usersInRoom[i] + "\"";
	}
	userJson += "}";
	return makeJsonResponse(response.response, userJson);
}

/*
 This function serializes a question and its answer options if available.
 param: response - the GetQuestionResponse object
 return: std::string - JSON with question and answers or status only
 time: O(n)
*/
std::string JsonPacketSerializer::serializeResponse(const GetQuestionResponse& response)
{
	if (response.response == std::string(GET_QUESTION) + SUCCESS_RESPONSE)
	{
		std::string answersJson = "{";
		bool first = true;
		for (const auto& pair : response.answers)
		{
			if (!first) answersJson += ", ";
			first = false;
			answersJson += "\"" + std::to_string(pair.first) + "\": \"" + pair.second + "\"";
		}
		answersJson += "}";
		std::string data = "{\"" QUESTION_KEY "\": \"" + response.question + "\", \"" ANSWERS_KEY "\": " + answersJson + "}";
		return makeJsonResponse(response.response, data);
	}
	else
	{
		return makeJsonResponse(response.response);
	}
}

/*
 This function serializes the results of a game including correct/wrong counts and times.
 param: response - the GetGameResultsResponse object
 return: std::string - JSON-formatted results
 time: O(n)
*/
std::string JsonPacketSerializer::serializeResponse(const GetGameResultsResponse& response)
{
	std::string resultJson = "{";
	for (size_t i = 0; i < response.results.size(); ++i)
	{
		const auto& res = response.results[i];
		if (i > 0) resultJson += ", ";
		resultJson += "\"" + std::to_string(i) + "\": {\"" USERNAME_KEY "\": \"" + res.username +
			"\", \"" CORRECT_COUNT_KEY "\": " + std::to_string(res.correctAnswerCount) +
			", \"" WRONG_COUNT_KEY "\": " + std::to_string(res.wrongAnswerCount) +
			", \"" AVG_TIME_KEY "\": " + std::to_string(res.averageAnswerTime) + "}";
	}
	resultJson += "}";
	return makeJsonResponse(response.response, resultJson);
}

/*
 This function serializes the answer submission result, including correct answer ID if correct.
 param: response - the SubmitAnswerResponse object
 return: std::string - JSON response
 time: O(1)
*/
std::string JsonPacketSerializer::serializeResponse(const SubmitAnswerResponse& response)
{
	std::string data = (response.response == std::string(SUBMIT_ANSWER) + SUCCESS_RESPONSE) ?
		"{\"" CORRECT_ANS_ID_KEY "\": " + std::to_string(response.correctAnswerId) + "}" : "{}";
	return makeJsonResponse(response.response, data);
}

/*
 This function serializes a user's statistics like scores and average time.
 param: response - the GetUserStatsResponse object
 return: std::string - JSON stats object
 time: O(1)
*/
std::string JsonPacketSerializer::serializeResponse(const GetUserStatsResponse& response)
{
	std::string data = "{\"" HIGHSCORE_KEY "\": " + std::to_string(response.highScore) +
		", \"" CORRECT_ANS_TOTAL_KEY "\": " + std::to_string(response.correctAnswerCount) +
		", \"" WRONG_ANS_TOTAL_KEY "\": " + std::to_string(response.wrongAnswerCount) +
		", \"avg_time\": " + std::to_string(response.avgTime) + "}";
	return makeJsonResponse(response.response, data);
}

/*
 This function detects the real type of a RequestResult and dispatches serialization to the correct function.
 param: req - the base pointer to a response object
 return: std::string - JSON-formatted serialized response
 time: O(n)
*/
std::string JsonPacketSerializer::serializeResponse(const RequestResult* req)
{
	std::string type;
	for (char c : req->response)
	{
		if (c == ':') break;
		type += c;
	}

	if (type == LOGIN) return serializeResponse(*(LoginResponse*)req);
	else if (type == SIGNUP) return serializeResponse(*(SignupResponse*)req);
	else if (type == SIGNOUT) return serializeResponse(*(SignoutResponse*)req);
	else if (type == CREATE_ROOM) return serializeResponse(*(CreateRoomResponse*)req);
	else if (type == GET_HIGHSCORES) return serializeResponse(*(HighscoreResponse*)req);
	else if (type == JOIN_ROOM) return serializeResponse(*(JoinRoomResponse*)req);
	else if (type == GET_PLAYERS_IN_ROOM) return serializeResponse(*(GetPlayersInRoomResponse*)req);
	else if (type == GET_ROOMS) return serializeResponse(*(GetRoomsResponse*)req);
	else if (type == GET_ROOM_STATE) return serializeResponse(*(GetRoomStateResponse*)req);
	else if (type == CLOSE_ROOM) return serializeResponse(*(CloseRoomResponse*)req);
	else if (type == START_GAME) return serializeResponse(*(StartGameResponse*)req);
	else if (type == LEAVE_ROOM) return serializeResponse(*(LeaveRoomResponse*)req);
	else if (type == GET_QUESTION) return serializeResponse(*(GetQuestionResponse*)req);
	else if (type == GET_GAME_RESULTS) return serializeResponse(*(GetGameResultsResponse*)req);
	else if (type == SUBMIT_ANSWER) return serializeResponse(*(SubmitAnswerResponse*)req);
	else if (type == GET_USER_STATS) return serializeResponse(*(GetUserStatsResponse*)req);
	else if (type == BACK_TO_MENU) return makeJsonResponse(BACK_TO_MENU + std::string(SUCCESS_RESPONSE));
	else if (req->response == ERROR_ROOM_SHUTDOWN) return makeJsonResponse(ERROR_ROOM_SHUTDOWN);
	else if (req->response == ERROR_GAME_STARTED) return makeJsonResponse(ERROR_GAME_STARTED);
	else return makeJsonResponse(INVALID_RESPONSE);
}
