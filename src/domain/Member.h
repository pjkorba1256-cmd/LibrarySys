#pragma once
#include <string>
#include <vector>
#include "User.h"

// ============================================================
//  Member — a library patron who can borrow books.
//  Extends User with borrow limits and fine tracking.
// ============================================================
class Member : public User {
private:
    static constexpr int    MAX_BOOKS  = 3;
    static constexpr double MAX_FINE   = 200.0; // Block borrowing above this

    std::vector<int> m_activeBorrowIds;
    double           m_totalFinesOwed = 0.0;

public:
    Member(int id, const std::string& name, const std::string& email);

    // ── Overrides ─────────────────────────────────────────────
    std::string getRole()        const override { return "MEMBER"; }
    bool        canBorrowBooks() const override;

    // ── Borrow tracking ───────────────────────────────────────
    void        addActiveBorrow(int transactionId);
    void        removeActiveBorrow(int transactionId);
    int         getActiveBorrowCount() const;
    const std::vector<int>& getActiveBorrowIds() const { return m_activeBorrowIds; }

    // ── Fine management (encapsulated) ─────────────────────────
    void   addFine(double amount);
    void   clearFine(double amount);  // partial or full payment
    double getTotalFinesOwed() const { return m_totalFinesOwed; }
    void   setFinesOwed(double amount) { m_totalFinesOwed = amount; }
    void   setActiveBorrows(const std::vector<int>& ids) { m_activeBorrowIds = ids; }
};
