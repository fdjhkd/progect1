#pragma once
#include "LoggedUser.h"
#include "IDatabase.h"
#include <vector>
#include <string>

#define ERR_INVALID_USERNAME "Invalid characters in username"
#define ERR_INVALID_PASSWORD "Invalid characters in password"
#define ERR_INVALID_EMAIL "Invalid characters in email"
#define ERR_USER_EXISTS "User already exists"
#define ERR_USER_NOT_EXIST "User does not exist"
#define ERR_PASSWORD_MISMATCH "Password mismatch"
#define ERR_ALREADY_LOGGED "User already logged in"
#define ERR_USER_NOT_LOGGED "User not logged"

class LoginManager
{
private:
    std::vector<LoggedUser> m_loggedUsers;
    IDatabase m_database;

    std::string cleanUsername(const std::string& username);
    bool isStringValid(const std::string& str);
    bool isLegalChar(char c);

public:
    LoginManager();

    void signup(std::string username, std::string password, std::string email);
    void login(std::string username, std::string password);
    void logout(std::string username);
    void shutDown();
    bool isLogged(std::string username);
    void printLoggedUsers();
    IDatabase& getDatabase();
};

