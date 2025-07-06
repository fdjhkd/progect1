#include "LoginManager.h"
#include <iostream>
#include <cctype>

LoginManager::LoginManager() : m_loggedUsers(std::vector<LoggedUser>()), m_database(IDatabase())
{
}


/*
 This function registers a new user if username, password, and email are valid and unique.
 param:
    username - desired username
    password - desired password
    param email - user's email
 time: O(n) where n = length of strings (username, password, email)
*/

void LoginManager::signup(std::string username, std::string password, std::string email)
{
    username = cleanUsername(username);

    if (!isStringValid(username))
        throw std::exception(ERR_INVALID_USERNAME);

    if (!isStringValid(password))
        throw std::exception(ERR_INVALID_PASSWORD);

    if (!isStringValid(email))
        throw std::exception(ERR_INVALID_EMAIL);

    if (m_database.doesUserExist(username))
        throw std::exception(ERR_USER_EXISTS);

    m_database.addUser(username, password, email);
}


/*
 This function logs a user in if credentials are correct and user isn't already logged in.
 paras:
     username - the username to log in
     password - the password to authenticate
 Time: O(n) where n = number of currently logged in users
*/

void LoginManager::login(std::string username, std::string password)
{
    if (!m_database.doesUserExist(username))
        throw std::exception(ERR_USER_NOT_EXIST);

    if (isLogged(username))
        throw std::exception(ERR_ALREADY_LOGGED);

    std::string correctPassword = m_database.getPassword(username);
    if (correctPassword != password)
        throw std::exception(ERR_PASSWORD_MISMATCH);

    m_loggedUsers.push_back(LoggedUser(username));
}


/*
 This function logs a user out by removing them from the list of logged in users.
 param username - the username to log out
 time: O(n) where n = number of currently logged in users
*/
void LoginManager::logout(std::string username)
{
    if (!m_database.doesUserExist(username))
        throw std::exception(ERR_USER_NOT_EXIST);

    for (auto it = m_loggedUsers.begin(); it != m_loggedUsers.end(); ++it)
    {
        if (it->getUsername() == username)
        {
            m_loggedUsers.erase(it);
            return;
        }
    }

    throw std::exception(ERR_USER_NOT_LOGGED);
}

/*
 This function shuts down the database safely.
 return None
 time: O(1)
*/

void LoginManager::shutDown()
{
    m_database.close();
}


/*
 This function checks if a user is currently logged in.
 param:
     username - the username to check
 return true if the user is logged in, false otherwise
 time: O(n) where n = number of currently logged in users
 */

bool LoginManager::isLogged(std::string username)
{
    for (const auto& user : m_loggedUsers)
    {
        if (user.getUsername() == username)
            return true;
    }
    return false;
}


/*
 This function prints all currently logged in users to the standard output.
 return none
 time: O(n) where n = number of currently logged in users
*/

void LoginManager::printLoggedUsers()
{
    for (const auto& user : m_loggedUsers)
    {
        std::cout << user.getUsername() << std::endl;
    }
}


/*
 This function returns a reference to the database object used.
 return IDatabase& - reference to the database
 Time: O(1)
*/

IDatabase& LoginManager::getDatabase()
{
    return m_database;
}



/*
 This function removes all illegal characters from a username.
 param username - the original username input
 return std::string - cleaned username with only allowed characters
 Time: O(n) where n = length of the input string
*/

std::string LoginManager::cleanUsername(const std::string& username)
{
    std::string cleaned;
    for (char c : username)
    {
        if (c != '"' && (std::isalnum(static_cast<unsigned char>(c)) || isLegalChar(c)))
        {
            cleaned += c;
        }
    }
    return cleaned;
}


/*
 This function checks if a string contains only allowed characters.
 param str - the string to validate
 return true if valid, false otherwise
 time: O(n) where n = length of the string
*/

bool LoginManager::isStringValid(const std::string& str)
{
    for (char c : str)
    {
        if (c == '"')
            return false;
        if (!std::isalnum(static_cast<unsigned char>(c)) && !isLegalChar(c))
            return false;
    }
    return true;
}


/*
 This function checks whether a character is in the set of allowed special characters.
 param c - the character to check
 return true if allowed, false otherwise
 Time: O(1)
*/

bool LoginManager::isLegalChar(char c)
{
    return c == '!' || c == '?' || c == '$' || c == '@' ||
        c == '.' || c == '_' || c == '(' || c == ')';
}
