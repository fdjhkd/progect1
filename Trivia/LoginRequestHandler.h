#pragma once

#include "LoginManager.h"
#include "RequestStructs.h"
#include "json.hpp"
#include "IRequestHandler.h"
#include "JsonPacketDeserializer.h"
#include "JsonPacketSerializer.h"


// JSON field keys
#define FIELD_TYPE "type"
#define FIELD_USERNAME "username"
#define FIELD_PASSWORD "password"
#define FIELD_EMAIL "email"

// Request types
#define REQ_TYPE_LOGIN "login"
#define REQ_TYPE_SIGNUP "signup"

// Responses
#define RESP_SUCCESS "success"
#define RESP_INVALID "invalid"
#define RESP_USER_LOGGED_IN "err_user_logged_in"
#define RESP_USER_DOESNT_EXIST "err_user_doesnt_exist"
#define RESP_PASSWORD_MISMATCH "err_password_mismatch"
#define RESP_USER_EXISTS "err_user_exist"
#define RESP_INVALID_USERNAME "err_invalid_input_username"
#define RESP_INVALID_PASSWORD "err_invalid_input_password"
#define RESP_INVALID_EMAIL "err_invalid_input_email"

// Exception messages
#define EXC_USER_LOGGED_IN "User already logged in"
#define EXC_USER_DOESNT_EXIST "User does not exist"
#define EXC_PASSWORD_MISMATCH "Password mismatch"
#define EXC_USER_EXISTS "User already exists"
#define EXC_INVALID_USERNAME "Invalid characters in username"
#define EXC_INVALID_PASSWORD "Invalid characters in password"
#define EXC_INVALID_EMAIL "Invalid characters in email"

class RequestHandlerFactory;
class LoginRequestHandler : public IRequestHandler
{
private:
	RequestHandlerFactory* m_loginFactory;
	LoginManager& m_loginManager;

	std::string getJsonStringField(const nlohmann::json& json, const std::string& key);
	bool isLoginRequest(const nlohmann::json& json);
	bool isSignupRequest(const nlohmann::json& json);
	bool validateCredentials(const std::string& username, const std::string& password);
	bool validateSignupFields(const std::string& username, const std::string& password, const std::string& email);

	RequestResult* processSignup(const Request& req, const nlohmann::json& jsonData);
	RequestResult* processLogin(const Request& req, const nlohmann::json& jsonData);

public:
	bool isRequestRelevant(const Request& req) override;
	RequestResult* handleRequest(const Request& req) override;
	RequestResult login(Request req);
	RequestResult signup(Request req);

	LoginRequestHandler(RequestHandlerFactory* loginFactory, LoginManager& loginManager);
	~LoginRequestHandler();
	void shutDown();
};

