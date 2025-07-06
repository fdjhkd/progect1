#include "RoomMemberRequestHandler.h"

/*
 Constructor for RoomMemberRequestHandler.
 param: room - reference to the room object
 param: user - the currently logged-in user
 param: roomManager - the room manager instance
 param: handlerFactory - factory for generating new handlers
 time: O(1)
*/
RoomMemberRequestHandler::RoomMemberRequestHandler(Room& room, const LoggedUser& user, RoomManager& roomManager, RequestHandlerFactory* handlerFactory)
    : m_room(room), m_user(user), m_roomManager(roomManager), m_handlerFacroty(handlerFactory)
{
}

/*
 Validates if buffer is a valid JSON.
 param: buffer - string to validate
 return: true if valid JSON, false otherwise
 time: O(n)
*/
bool RoomMemberRequestHandler::isJsonValid(const std::string& buffer) const
{
    try {
        nlohmann::json::parse(buffer);
        return true;
    }
    catch (...) {
        return false;
    }
}

/*
 Extracts the type field from JSON buffer.
 param: buffer - JSON string
 return: string with the type value
 time: O(n)
*/
std::string RoomMemberRequestHandler::getRequestType(const std::string& buffer) const
{
    nlohmann::json jsonData = nlohmann::json::parse(buffer);
    return jsonData[REQUEST_TYPE];
}

/*
 Validates that the room still exists in manager.
 param: ret - pointer to RequestResult to update response if invalid
 return: true if room exists, false otherwise
 time: O(log n)
*/
bool RoomMemberRequestHandler::isRoomValid(RequestResult*& ret)
{
    try {
        m_roomManager.getRoomById(m_roomId);
    }
    catch (...) {
        ret->response = ERR_ROOM_SHUTDOWN;
        return false;
    }
    return true;
}

/*
 Checks if game already started in room.
 param: ret - pointer to RequestResult to update response if game started
 return: true if active, false otherwise
 time: O(1)
*/
bool RoomMemberRequestHandler::isGameStarted(RequestResult*& ret)
{
    if (m_room.getRoomData().isActive)
    {
        ret->response = ERR_GAME_STARTED;
        return true;
    }
    return false;
}

/*
 Handles a leave room request.
 param: result - pointer to RequestResult to update
 param: req - original request
 time: O(1)
*/
void RoomMemberRequestHandler::handleLeaveRoom(RequestResult*& result, const Request& req)
{
    *result = leaveRoom(req);
    result->response = LEAVE_ROOM_RESPONSE + result->response;
}

/*
 Handles get room state request.
 param: result - pointer to RequestResult to delete
 param: req - original request
 return: pointer to GetRoomStateResponse
 time: O(m), where m = number of users in room
*/
RequestResult* RoomMemberRequestHandler::handleGetRoomState(RequestResult*& result, const Request& req)
{
    delete result;
    GetRoomStateResponse* grsr = getRoomState(req);
    grsr->response = GET_ROOM_STATE_RESPONSE + grsr->response;
    return grsr;
}

/*
 Checks if request is a non-empty valid JSON.
 param: req - incoming request
 return: true if valid, false otherwise
 time: O(n)
*/
bool RoomMemberRequestHandler::isRequestRelevant(const Request& req)
{
    return !req.buffer.empty() && isJsonValid(req.buffer);
}

/*
 Returns current room state.
 param: req - request object
 return: pointer to GetRoomStateResponse
 time: O(m)
*/
GetRoomStateResponse* RoomMemberRequestHandler::getRoomState(const Request& req)
{
    RoomData data = m_room.getRoomData();
    std::vector<std::string> users;
    for (auto& user : m_room.getAllUsers())
        users.push_back(user.getUsername());

    return new GetRoomStateResponse("success", this, m_user.getUsername(), SUCCESS, data.isActive, users, data.questionCount, data.timePerQuestion);
}

/*
 Handles leaving the room.
 param: req - request object
 return: LeaveRoomResponse with status
 time: O(1)
*/
LeaveRoomResponse RoomMemberRequestHandler::leaveRoom(const Request& req)
{
    m_room.removeUser(m_user);
    return LeaveRoomResponse("success", this, m_user.getUsername(), SUCCESS);
}

/*
 Central function that handles incoming requests.
 param: req - incoming request
 return: pointer to RequestResult or derived response
 time: O(log n + m)
*/
RequestResult* RoomMemberRequestHandler::handleRequest(const Request& req)
{
    RequestResult* result = new RequestResult();
    result->requestHandler = nullptr;
    result->response = INVALID_RESPONSE;
    result->username = m_user.getUsername();

    if (!isRoomValid(result) || isGameStarted(result))
        return result;

    if (isRequestRelevant(req))
    {
        std::string type = getRequestType(req.buffer);
        if (type == GET_ROOM_STATE)
            return handleGetRoomState(result, req);
        else if (type == LEAVE_ROOM)
            handleLeaveRoom(result, req);
    }

    return result;
}
