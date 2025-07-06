#include "LoggedUser.h"

/*
 This function constructs a LoggedUser object using the provided username.
 param username - the name of the user to associate with this object
 time: O(1)
*/
LoggedUser::LoggedUser(std::string username) : m_username(username)
{

}

/*
 This function returns the username of the logged-in user.
 return std::string - the stored username
 time: O(1)
 */
std::string LoggedUser::getUsername() const
{
	return m_username;
}

/*
 This function sets or updates the username of the logged-in user.
 param username - the new username to assign
 Time: O(1)
*/
void LoggedUser::setUsername(const std::string& username)
{
	m_username = username;
}