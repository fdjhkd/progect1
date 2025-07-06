#include "JsonPacketDeserializer.h"
#include "json.hpp"


/*
 This function deserializes a login request from a JSON buffer.
 param: req - the raw request containing JSON data
 return: LoginRequest - structured login request data
 time: O(1)
*/
LoginRequest JsonPacketDeserializer::deserializeLoginRequest(const Request& req)
{
	nlohmann::json jsonData = nlohmann::json::parse(req.buffer);
	return LoginRequest(req.id, req.recivalTime, req.buffer, jsonData[JSON_KEY_USERNAME], jsonData[JSON_KEY_PASSWORD]);
}

/*
 This function deserializes a signup request from a JSON buffer.
 param: req - the raw request containing JSON data
 return: SignupRequest - structured signup request data
 time: O(1)
*/
SignupRequest JsonPacketDeserializer::deserializeSignupRequest(const Request& req)
{
	nlohmann::json jsonData = nlohmann::json::parse(req.buffer);
	return SignupRequest(req.id, req.recivalTime, req.buffer, jsonData[JSON_KEY_USERNAME], jsonData[JSON_KEY_PASSWORD], jsonData[JSON_KEY_EMAIL]);
}

/*
 This function deserializes a get players in room request.
 param: req - the raw request with JSON containing room_id
 return: GetPlayersInRoomRequest - structured room ID request
 time: O(1)
*/
GetPlayersInRoomRequest JsonPacketDeserializer::deserializeGetPlayersRequest(const Request& req)
{
	nlohmann::json jsonData = nlohmann::json::parse(req.buffer);
	if (jsonData[JSON_KEY_ROOM_ID] != "null")
	{
		return GetPlayersInRoomRequest(req.id, req.recivalTime, req.buffer, jsonData[JSON_KEY_ROOM_ID]);
	}
	throw std::exception(ERR_MISSING_GET_PLAYERS);
}

/*
 This function deserializes a join room request from JSON.
 param: req - the raw request with room_id in JSON
 return: JoinRoomRequest - structured join room request
 time: O(1)
*/
JoinRoomRequest JsonPacketDeserializer::deserializeJoinRoomRequest(const Request& req)
{
	nlohmann::json jsonData = nlohmann::json::parse(req.buffer);
	if (jsonData[JSON_KEY_ROOM_ID] != "null")
	{
		return JoinRoomRequest(req.id, req.recivalTime, req.buffer, jsonData[JSON_KEY_ROOM_ID]);
	}
	throw std::exception(ERR_MISSING_JOIN_ROOM);
}

/*
 This function deserializes a create room request and validates inputs.
 param: req - the raw request with room settings in JSON
 return: CreateRoomRequest - structured room creation request
 time: O(n), where n is the length of the room name
*/
CreateRoomRequest JsonPacketDeserializer::deserializeCreateRoomRequest(const Request& req)
{
	nlohmann::json jsonData = nlohmann::json::parse(req.buffer);
	if ((jsonData[JSON_KEY_ROOM_NAME] != "null" && jsonData[JSON_KEY_MAX_USERS] != "null" && jsonData[JSON_KEY_QUESTION_COUNT] != "null" && jsonData[JSON_KEY_ANSWER_TIMEOUT] != "null") &&
		(jsonData[JSON_KEY_MAX_USERS] > 0 && jsonData[JSON_KEY_QUESTION_COUNT] > 0 && jsonData[JSON_KEY_ANSWER_TIMEOUT] > 0))
	{
		std::string roomName = jsonData[JSON_KEY_ROOM_NAME];
		for (char c : roomName)
		{
			if (!IS_LEGAL_CHAR(c))
				throw std::exception(ERR_INVALID_ROOM_NAME);
		}
		return CreateRoomRequest(req.id, req.recivalTime, req.buffer,
			jsonData[JSON_KEY_ROOM_NAME],
			jsonData[JSON_KEY_MAX_USERS],
			jsonData[JSON_KEY_QUESTION_COUNT],
			jsonData[JSON_KEY_ANSWER_TIMEOUT]);
	}
	throw std::exception(ERR_MISSING_CREATE_PARAMS);
}

/*
 This function deserializes a submitted answer from JSON.
 param: req - the raw request with answer_id in JSON
 return: SubmitAnswerRequest - structured submitted answer
 time: O(1)
*/
SubmitAnswerRequest JsonPacketDeserializer::deserializeSubmitAnswerRequest(const Request& req)
{
	nlohmann::json jsonData = nlohmann::json::parse(req.buffer);
	if (jsonData.contains(JSON_KEY_ANSWER_ID) && jsonData[JSON_KEY_ANSWER_ID] != "null")
	{
		int answerId = jsonData[JSON_KEY_ANSWER_ID];
		if (answerId >= ANSWER_DOWN_LIM && answerId <= ANSWER_UP_LIM)
		{
			return SubmitAnswerRequest(req.id, req.recivalTime, req.buffer, answerId);
		}
		throw std::exception(ERR_INVALID_ANSWER_ID);
	}
	throw std::exception(ERR_NO_ANSWER_ID);
}
