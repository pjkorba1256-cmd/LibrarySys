#include "Transaction.h"
#include "FineCalculator.h"
#include <ctime>
#include <sstream>
#include <iomanip>

Transaction::Transaction(int id, int bookId, int memberId)
    : m_id(id), m_bookId(bookId), m_memberId(memberId),
      m_returnDate(0), m_status(TransactionStatus::ACTIVE),
      m_fineCharged(0.0)
{
    m_borrowDate = std::time(nullptr);
    // Due date = borrow date + LOAN_PERIOD_DAYS
    m_dueDate = m_borrowDate + static_cast<std::time_t>(FineCalculator::LOAN_PERIOD_DAYS) * 86400;
}

int Transaction::calculateOverdueDays() const {
    std::time_t now = (m_returnDate != 0) ? m_returnDate : std::time(nullptr);
    if (now <= m_dueDate) return 0;
    return static_cast<int>((now - m_dueDate) / 86400);
}

double Transaction::computeFine() const {
    int days = calculateOverdueDays();
    return FineCalculator::calculate(days);
}

bool Transaction::isOverdue() const {
    return std::time(nullptr) > m_dueDate && m_status == TransactionStatus::ACTIVE;
}

void Transaction::markReturned() {
    m_returnDate  = std::time(nullptr);
    m_fineCharged = computeFine();
    m_status      = TransactionStatus::RETURNED;
}

void Transaction::markLost() {
    m_status      = TransactionStatus::LOST;
    m_fineCharged = FineCalculator::MAX_FINE; // maximum penalty
}

// ── String conversion helpers ─────────────────────────────────────────────
std::string Transaction::statusToString(TransactionStatus s) {
    switch (s) {
        case TransactionStatus::ACTIVE:   return "ACTIVE";
        case TransactionStatus::RETURNED: return "RETURNED";
        case TransactionStatus::OVERDUE:  return "OVERDUE";
        case TransactionStatus::LOST:     return "LOST";
        default:                          return "UNKNOWN";
    }
}

TransactionStatus Transaction::stringToStatus(const std::string& s) {
    if (s == "RETURNED") return TransactionStatus::RETURNED;
    if (s == "OVERDUE")  return TransactionStatus::OVERDUE;
    if (s == "LOST")     return TransactionStatus::LOST;
    return TransactionStatus::ACTIVE;
}

std::string Transaction::timeToString(std::time_t t) {
    std::ostringstream oss;
    oss << t;
    return oss.str();
}

std::time_t Transaction::stringToTime(const std::string& s) {
    if (s.empty() || s == "0") return 0;
    return static_cast<std::time_t>(std::stoll(s));
}
