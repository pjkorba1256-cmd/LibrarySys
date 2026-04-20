#pragma once
#include <chrono>
#include <string>
#include <ctime>

// ============================================================
//  Transaction — models a single borrow/return lifecycle.
//  ALL fine calculation and status transitions are encapsulated
//  here. Callers never perform date math directly.
// ============================================================
enum class TransactionStatus { ACTIVE, RETURNED, OVERDUE, LOST };

class Transaction {
private:
    int               m_id;
    int               m_bookId;
    int               m_memberId;
    std::time_t       m_borrowDate;
    std::time_t       m_dueDate;
    std::time_t       m_returnDate;   // 0 if not yet returned
    TransactionStatus m_status;
    double            m_fineCharged;  // finalized fine on return

    static constexpr int    LOAN_PERIOD_DAYS = 14;
    static constexpr double FINE_PER_DAY     = 5.0; // ₹5 per day

    // Private helper — callers cannot see date math
    int calculateOverdueDays() const;

public:
    Transaction(int id, int bookId, int memberId);
    Transaction() = default;

    // ── Encapsulated domain behaviour ─────────────────────────
    double computeFine()   const;  // live calculation; always up to date
    bool   isOverdue()     const;
    void   markReturned();         // sets returnDate, finalizes fine, updates status
    void   markLost();

    // ── Getters ───────────────────────────────────────────────
    int               getId()          const { return m_id; }
    int               getBookId()      const { return m_bookId; }
    int               getMemberId()    const { return m_memberId; }
    std::time_t       getBorrowDate()  const { return m_borrowDate; }
    std::time_t       getDueDate()     const { return m_dueDate; }
    std::time_t       getReturnDate()  const { return m_returnDate; }
    TransactionStatus getStatus()      const { return m_status; }
    double            getFineCharged() const { return m_fineCharged; }

    // ── Setters (for persistence restore) ─────────────────────
    void setId(int id)                      { m_id = id; }
    void setBookId(int id)                  { m_bookId = id; }
    void setMemberId(int id)                { m_memberId = id; }
    void setBorrowDate(std::time_t t)       { m_borrowDate = t; }
    void setDueDate(std::time_t t)          { m_dueDate = t; }
    void setReturnDate(std::time_t t)       { m_returnDate = t; }
    void setStatus(TransactionStatus s)     { m_status = s; }
    void setFineCharged(double f)           { m_fineCharged = f; }

    // ── Utility ───────────────────────────────────────────────
    static std::string statusToString(TransactionStatus s);
    static TransactionStatus stringToStatus(const std::string& s);
    static std::string timeToString(std::time_t t);
    static std::time_t stringToTime(const std::string& s);
};
