#include "User.hpp"

User::User(Id user_id, const std::string& login, const std::string& password) : m_user_data({user_id, login, password}){
}
