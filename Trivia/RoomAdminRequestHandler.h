#pragma once

#include "IRequestHandler.h"
#include "RequestStructs.h"
#include "RoomManager.h"
#include "json.hpp"

#define REQUEST_TYPE "type"
#define CLOSE_ROOM "close_room"
#define START_GAME "start_game"
#define GET_ROOM_STATE "get_room_state"
#define CLOSE_ROOM_RESPONSE "close_room: "
#define START_GAME_RESPONSE "start_game: "
#define GET_ROOM_STATE_RESPONSE "get_room_state: "

class RequestHandlerFactory;

class RoomAdminRequestHandler : public IRequestHandler
{
private:
	Room& m_room;
	RoomManager& m_roomManager;
	LoggedUser m_user;
	RequestHandlerFactory* m_handlerFactory;

	bool isJsonValid(const std::string& buffer) const;

	std::string getRequestType(const std::string& buffer) const;

	void handleCloseRoom(RequestResult*& result, const Request& req);

	void handleStartGame(RequestResult*& result, const Request& req);

	RequestResult* handleGetRoomState(RequestResult*& result, const Request& req);

public:
	RoomAdminRequestHandler(RequestHandlerFactory* handlerFactory, Room& room, RoomManager& roomManager, const LoggedUser& user);

	virtual bool isRequestRelevant(const Request& req) override;

	virtual RequestResult* handleRequest(const Request& req) override;

	GetRoomStateResponse* getRoomState(const Request& req);

	StartGameResponse startGame(const Request& req);

	CloseRoomResponse closeRoom(const Request& req);
};