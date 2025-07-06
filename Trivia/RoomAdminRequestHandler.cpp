#include "RoomAdminRequestHandler.h"

/*
 Constructor for RoomAdminRequestHandler.
 param: handlerFactory - factory to create other handlers
 param: room - reference to the managed room
 param: roomManager - reference to room manager
 param: user - currently logged-in admin user
 time: O(1)
*/
RoomAdminRequestHandler::RoomAdminRequestHandler(RequestHandlerFactory* handlerFactory, Room& room, RoomManager& roomManager, const LoggedUser& user)
	: m_handlerFactory(handlerFactory), m_user(user), m_room(room), m_roomManager(roomManager)
{
}

/*
 Validates whether the provided string is valid JSON.
 param: buffer - string containing JSON
 return: true if valid JSON, false otherwise
 time: O(n)
*/
bool RoomAdminRequestHandler::isJsonValid(const std::string& buffer) const
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
 Extracts the request type from JSON string.
 param: buffer - stringified JSON
 return: value of the "type" field
 time: O(n)
*/
std::string RoomAdminRequestHandler::getRequestType(const std::string& buffer) const
{
	nlohmann::json jsonData = nlohmann::json::parse(buffer);
	return jsonData[REQUEST_TYPE];
}

/*
 Checks whether request is valid and has valid JSON.
 param: req - the request object
 return: true if relevant, false otherwise
 time: O(n)
*/
bool RoomAdminRequestHandler::isRequestRelevant(const Request& req)
{
	return !req.buffer.empty() && isJsonValid(req.buffer);
}

/*
 Removes the room from the room manager.
 param: req - close room request
 return: CloseRoomResponse with result info
 time: O(log n)
*/
CloseRoomResponse RoomAdminRequestHandler::closeRoom(const Request& req)
{
	m_roomManager.deleteRoom(m_room.getRoomData().name);
	return CloseRoomResponse("success", this, m_user.getUsername(), SUCCESS);
}

/*
 Starts the game in the room if it's not already active.
 param: req - start game request
 return: StartGameResponse with result info
 time: O(1)
*/
StartGameResponse RoomAdminRequestHandler::startGame(const Request& req)
{
	StartGameResponse response("invalid", nullptr, m_user.getUsername(), FAIL);
	if (m_room.getRoomData().isActive)
		response.response = "err_room_already_active";
	else {
		m_room.activateRoom();
		response.response = "success";
		response.status = SUCCESS;
		response.requestHandler = this;
	}
	return response;
}

/*
 Retrieves current state of the room.
 param: req - get room state request
 return: pointer to dynamically allocated GetRoomStateResponse
 time: O(m), where m is number of users in room
*/
GetRoomStateResponse* RoomAdminRequestHandler::getRoomState(const Request& req)
{
	RoomData data = m_room.getRoomData();
	std::vector<std::string> users;
	for (const LoggedUser& user : m_room.getAllUsers())
		users.push_back(user.getUsername());

	return new GetRoomStateResponse("success", this, m_user.getUsername(), SUCCESS, data.isActive, users, data.questionCount, data.timePerQuestion);
}

/*
 Processes close room request and sets response.
 param: result - pointer to result object to modify
 param: req - incoming request
 time: O(log n)
*/
void RoomAdminRequestHandler::handleCloseRoom(RequestResult*& result, const Request& req)
{
	*result = closeRoom(req);
	result->response = CLOSE_ROOM_RESPONSE + result->response;
}

/*
 Processes start game request and sets response.
 param: result - pointer to result object to modify
 param: req - incoming request
 time: O(1)
*/
void RoomAdminRequestHandler::handleStartGame(RequestResult*& result, const Request& req)
{
	*result = startGame(req);
	result->response = START_GAME_RESPONSE + result->response;
}

/*
 Processes get room state request.
 param: result - pointer to current result (to be deleted)
 param: req - incoming request
 return: pointer to GetRoomStateResponse
 time: O(m)
*/
RequestResult* RoomAdminRequestHandler::handleGetRoomState(RequestResult*& result, const Request& req)
{
	delete result;
	GetRoomStateResponse* grsr = getRoomState(req);
	grsr->response = GET_ROOM_STATE_RESPONSE + grsr->response;
	return grsr;
}

/*
 Main function that handles incoming request routing.
 param: req - request object
 return: pointer to request result
 time: O(log n + m)
*/
RequestResult* RoomAdminRequestHandler::handleRequest(const Request& req)
{
	RequestResult* result = new RequestResult;
	result->requestHandler = nullptr;
	result->response = "invalid";
	result->username = m_user.getUsername();

	if (!isRequestRelevant(req))
		return result;

	std::string type = getRequestType(req.buffer);

	if (type == CLOSE_ROOM)
		handleCloseRoom(result, req);
	else if (type == START_GAME)
		handleStartGame(result, req);
	else if (type == GET_ROOM_STATE)
		return handleGetRoomState(result, req);

	return result;
}
