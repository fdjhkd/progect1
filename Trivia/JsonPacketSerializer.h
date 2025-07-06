
#pragma once
#include "RequestStructs.h"
#include "json.hpp"
#define SUCCESS 100
#define FAIL 101
// General response macros
#define RESPONSE_KEY "response"
#define DATA_KEY "data"
// Response types
#define LOGIN "login"
#define SIGNUP "signup"
#define SIGNOUT "signout"
#define CREATE_ROOM "create_room"
#define JOIN_ROOM "join_room"
#define GET_ROOMS "get_rooms"
#define GET_PLAYERS_IN_ROOM "get_players_in_room"
#define GET_HIGHSCORES "get_highscores"
#define GET_ROOM_STATE "get_room_state"
#define CLOSE_ROOM "close_room"
#define START_GAME "start_game"
#define LEAVE_ROOM "leave_room"
#define GET_QUESTION "get_question"
#define GET_GAME_RESULTS "get_game_results"
#define SUBMIT_ANSWER "submit_answer"
#define GET_USER_STATS "get_user_stats"
#define BACK_TO_MENU "back_to_menu"
// Response messages
#define SUCCESS_RESPONSE ": success"
#define ERROR_ROOM_SHUTDOWN "err_room_shutdown"
#define ERROR_GAME_STARTED "err_game_started"
#define INVALID_RESPONSE "invalid"
// Common field keys
#define QUESTION_KEY "question"
#define ANSWERS_KEY "answers"
#define CORRECT_ANS_ID_KEY "correct_ans_id"
#define USERS_KEY "players"
#define USERNAME_KEY "username"
#define MAX_PLAYERS_KEY "max_players"
#define QUESTION_COUNT_KEY "question_count"
#define QUESTION_TIMEOUT_KEY "question_timeout"
#define IS_ACTIVE_KEY "is_active"
#define HAS_GAME_BEGUN_KEY "has_game_begun"
#define AVG_TIME_KEY "average_time"
#define CORRECT_COUNT_KEY "correct_answers"
#define WRONG_COUNT_KEY "wrong_answers"
#define HIGHSCORE_KEY "highscore"
#define CORRECT_ANS_TOTAL_KEY "correct_ans_count"
#define WRONG_ANS_TOTAL_KEY "wrong_ans_count"
class JsonPacketSerializer
{
public:
	std::string serializeResponse(const ErrorResponse&);
	std::string serializeResponse(const LoginResponse&);
	std::string serializeResponse(const SignupResponse&);
	std::string serializeResponse(const SignoutResponse&);
	std::string serializeResponse(const GetRoomsResponse&);
	std::string serializeResponse(const JoinRoomResponse&);
	std::string serializeResponse(const CreateRoomResponse&);
	std::string serializeResponse(const HighscoreResponse&);
	std::string serializeResponse(const GetPlayersInRoomResponse&);
	std::string serializeResponse(const CloseRoomResponse&);
	std::string serializeResponse(const StartGameResponse&);
	std::string serializeResponse(const GetRoomStateResponse&);
	std::string serializeResponse(const LeaveRoomResponse&);
	std::string serializeResponse(const GetQuestionResponse&);
	std::string serializeResponse(const GetGameResultsResponse&);
	std::string serializeResponse(const SubmitAnswerResponse&);
	std::string serializeResponse(const GetUserStatsResponse&);
	std::string serializeResponse(const RequestResult*);
};

