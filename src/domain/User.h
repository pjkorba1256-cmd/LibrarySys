#pragma once
#include <string>

// ============================================================
//  Abstract base class for all system users.
//  Librarian and Member inherit from this.
// ============================================================
class User {
protected:
    int         m_id;
    std::string m_name;
    std::string m_email;
    std::string m_passwordHash;

public:
    User(int id, const std::string& name, const std::string& email);
    virtual ~User() = default;

    // Pure virtuals — subclasses define role identity & permissions
    virtual std::string getRole()        const = 0;
    virtual bool        canBorrowBooks() const = 0;

    // Shared concrete behavior
    int         getId()           const { return m_id; }
    std::string getName()         const { return m_name; }
    std::string getEmail()        const { return m_email; }
    void        setPasswordHash(const std::string& hash) { m_passwordHash = hash; }
    std::string getPasswordHash() const { return m_passwordHash; }
};
