#include "User.h"

User::User(int id, const std::string& name, const std::string& email)
    : m_id(id), m_name(name), m_email(email), m_passwordHash("") {}
