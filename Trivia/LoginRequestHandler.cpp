#include "LoginRequestHandler.h"

/*
 Extract a string field from JSON safely.
 time: O(1)
*/
std::string LoginRequestHandler::getJsonStringField(const nlohmann::json& json, const std::string& key)
{
	return json.contains(key) ? json[key].dump() : "null";
}

/*
 Check if request JSON is a login request.
 time: O(1)
*/
bool LoginRequestHandler::isLoginRequest(const nlohmann::json& json)
{
	return json.contains(FIELD_TYPE) && json[FIELD_TYPE] == REQ_TYPE_LOGIN;
}

/*
 Check if request JSON is a signup request.
 time: O(1)
*/
bool LoginRequestHandler::isSignupRequest(const nlohmann::json& json)
{
	return json.contains(FIELD_TYPE) && json[FIELD_TYPE] == REQ_TYPE_SIGNUP;
}

/*
 Check for valid credentials fields.
 time: O(1)
*/
bool LoginRequestHandler::validateCredentials(const std::string& username, const std::string& password)
{
	return username != "null" && password != "null";
}

/*
 Check for valid signup fields.
 time: O(1)
*/
bool LoginRequestHandler::validateSignupFields(const std::string& username, const std::string& password, const std::string& email)
{
	return validateCredentials(username, password) && email != "null";
}

/*
 Determine if this request is for login/signup.
 time: O(1)
*/
bool LoginRequestHandler::isRequestRelevant(const Request& req)
{
	try
	{
		nlohmann::json jsonData = nlohmann::json::parse(req.buffer);
		return isLoginRequest(jsonData) || isSignupRequest(jsonData);
	}
	catch (...)
	{
		return false;
	}
}

/*
 Main request handler entry point.
 time: O(1)
*/
RequestResult* LoginRequestHandler::handleRequest(const Request& req)
{
	RequestResult* ret = new RequestResult();

	if (!isRequestRelevant(req))
	{
		ret->response = RESP_INVALID;
		return ret;
	}

	nlohmann::json jsonData = nlohmann::json::parse(req.buffer);

	std::string username = getJsonStringField(jsonData, FIELD_USERNAME);
	std::string password = getJsonStringField(jsonData, FIELD_PASSWORD);
	std::string email = getJsonStringField(jsonData, FIELD_EMAIL);
	std::string type = jsonData[FIELD_TYPE];

	if (username == "null")
	{
		ret->response = RESP_INVALID;
		return ret;
	}

	if (type == REQ_TYPE_SIGNUP && validateSignupFields(username, password, email))
	{
		return processSignup(req, jsonData);
	}
	else if (type == REQ_TYPE_LOGIN && validateCredentials(username, password))
	{
		return processLogin(req, jsonData);
	}

	ret->response = RESP_INVALID;
	return ret;
}

/*
 Handle signup logic.
 time: O(1)
*/
RequestResult* LoginRequestHandler::processSignup(const Request& req, const nlohmann::json& jsonData)
{
	RequestResult* ret = new RequestResult();
	RequestResult temp = signup(req);

	if (temp.response == RESP_SUCCESS)
		login(req); // auto login after signup

	ret->response = std::string(REQ_TYPE_SIGNUP) + ": " + temp.response;
	ret->requestHandler = temp.requestHandler;
	ret->username = jsonData[FIELD_USERNAME].dump();

	return ret;
}

/*
 Handle login logic.
 time: O(1)
*/
RequestResult* LoginRequestHandler::processLogin(const Request& req, const nlohmann::json& jsonData)
{
	RequestResult* ret = new RequestResult();
	RequestResult temp = login(req);

	ret->response = std::string(REQ_TYPE_LOGIN) + ": " + temp.response;
	ret->requestHandler = temp.requestHandler;
	ret->username = jsonData[FIELD_USERNAME].dump();

	return ret;
}

/*
 Handle raw login logic.
 time: O(1)
*/
RequestResult LoginRequestHandler::login(Request req)
{
	JsonPacketDeserializer deserializer;
	LoginRequest logReq = deserializer.deserializeLoginRequest(req);
	RequestResult ret;
	ret.requestHandler = this;
	ret.response = RESP_INVALID;

	if (validateCredentials(logReq.username, logReq.password))
	{
		try
		{
			m_loginManager.login(logReq.username, logReq.password);
			ret.response = RESP_SUCCESS;
		}
		catch (const std::exception& exp)
		{
			std::string msg = exp.what();
			if (msg == EXC_USER_LOGGED_IN) ret.response = RESP_USER_LOGGED_IN;
			else if (msg == EXC_USER_DOESNT_EXIST) ret.response = RESP_USER_DOESNT_EXIST;
			else if (msg == EXC_PASSWORD_MISMATCH) ret.response = RESP_PASSWORD_MISMATCH;
		}
	}

	if (ret.response != RESP_SUCCESS)
		ret.requestHandler = nullptr;

	return ret;
}

/*
 Handle raw signup logic.
 time: O(1)
*/
RequestResult LoginRequestHandler::signup(Request req)
{
	JsonPacketDeserializer deserializer;
	SignupRequest signReq = deserializer.deserializeSignupRequest(req);
	RequestResult ret;
	ret.requestHandler = this;

	if (validateSignupFields(signReq.username, signReq.password, signReq.email))
	{
		try
		{
			m_loginManager.signup(signReq.username, signReq.password, signReq.email);
			ret.response = RESP_SUCCESS;
		}
		catch (const std::exception& exp)
		{
			std::string msg = exp.what();
			if (msg == EXC_USER_EXISTS) ret.response = RESP_USER_EXISTS;
			else if (msg == EXC_INVALID_USERNAME) ret.response = RESP_INVALID_USERNAME;
			else if (msg == EXC_INVALID_PASSWORD) ret.response = RESP_INVALID_PASSWORD;
			else if (msg == EXC_INVALID_EMAIL) ret.response = RESP_INVALID_EMAIL;
		}
	}

	if (ret.response != RESP_SUCCESS)
		ret.requestHandler = nullptr;

	return ret;
}

/*
 Constructor.
*/
LoginRequestHandler::LoginRequestHandler(RequestHandlerFactory* loginFactory, LoginManager& loginManager)
	: m_loginManager(loginManager)
{
	m_loginFactory = loginFactory;
}

/*
 Destructor for LoginRequestHandler.
*/
LoginRequestHandler::~LoginRequestHandler() { }

/*
 Shuts down login services.
*/
void LoginRequestHandler::shutDown()
{
	m_loginManager.shutDown();
}