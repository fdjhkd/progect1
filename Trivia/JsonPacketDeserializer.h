
#pragma once
#include "RequestStructs.h"
#define ANSWER_UP_LIM 3
#define ANSWER_DOWN_LIM 0
#define IS_LEGAL_CHAR(c) (isalnum(c) || (c) == '!' || (c) == '?' || (c) == '$' || (c) == '@' || (c) == '.' || (c) == '_' || (c) == '(' || (c) == ')' || (c) == ' ' || (c) == '\'')
#define JSON_KEY_USERNAME "username"
#define JSON_KEY_PASSWORD "password"
#define JSON_KEY_EMAIL "email"
#define JSON_KEY_ROOM_ID "room_id"
#define JSON_KEY_ROOM_NAME "room_name"
#define JSON_KEY_MAX_USERS "max_users"
#define JSON_KEY_QUESTION_COUNT "question_count"
#define JSON_KEY_ANSWER_TIMEOUT "answer_timeout"
#define JSON_KEY_ANSWER_ID "answer_id"
#define ERR_INVALID_ROOM_NAME "Invalid characters in room name"
#define ERR_MISSING_CREATE_PARAMS "Missing create room request parameters"
#define ERR_INVALID_ANSWER_ID "Invalid answer id submitted"
#define ERR_NO_ANSWER_ID "No answer id submitted"
#define ERR_MISSING_GET_PLAYERS "Missing get players in room request parameters"
#define ERR_MISSING_JOIN_ROOM "Missing join room request parameters"
class JsonPacketDeserializer
{
public:
	LoginRequest deserializeLoginRequest(const Request& req);
	SignupRequest deserializeSignupRequest(const Request& req);
	GetPlayersInRoomRequest deserializeGetPlayersRequest(const Request& req);
	JoinRoomRequest deserializeJoinRoomRequest(const Request& req);
	CreateRoomRequest deserializeCreateRoomRequest(const Request& req);
	SubmitAnswerRequest deserializeSubmitAnswerRequest(const Request& req);
};
