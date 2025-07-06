#pragma once
#include "RoomManager.h"
#include "IRequestHandler.h"
#include "json.hpp"
#include "RequestStructs.h"
#include "JsonPacketDeserializer.h"

#define REQUEST_TYPE "type"
#define GET_ROOM_STATE "get_room_state"
#define LEAVE_ROOM "leave_room"
#define GET_ROOM_STATE_RESPONSE "get_room_state: "
#define LEAVE_ROOM_RESPONSE "leave_room: "
#define ERR_GAME_STARTED "err_game_started"
#define ERR_ROOM_SHUTDOWN "err_room_shutdown"
#define INVALID_RESPONSE "invalid"

class RequestHandlerFactory;

class RoomMemberRequestHandler : public IRequestHandler
{
private:
    Room& m_room;
    LoggedUser m_user;
    RoomManager& m_roomManager;
    RequestHandlerFactory* m_handlerFacroty;
    unsigned int m_roomId;

    bool isJsonValid(const std::string& buffer) const;
    std::string getRequestType(const std::string& buffer) const;
    bool isRoomValid(RequestResult*& ret);
    bool isGameStarted(RequestResult*& ret);
    void handleLeaveRoom(RequestResult*& result, const Request& req);
    RequestResult* handleGetRoomState(RequestResult*& result, const Request& req);

public:
    RoomMemberRequestHandler(Room& room, const LoggedUser& user, RoomManager& roomManager, RequestHandlerFactory* handlerFactory);

    virtual bool isRequestRelevant(const Request& req) override;
    virtual RequestResult* handleRequest(const Request& req) override;

    LeaveRoomResponse leaveRoom(const Request&);
    GetRoomStateResponse* getRoomState(const Request&);
};


