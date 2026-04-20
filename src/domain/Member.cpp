#include "Member.h"
#include <algorithm>

Member::Member(int id, const std::string& name, const std::string& email)
    : User(id, name, email) {}

bool Member::canBorrowBooks() const {
    // Block if at borrow limit OR excessive fines
    if (static_cast<int>(m_activeBorrowIds.size()) >= MAX_BOOKS) return false;
    if (m_totalFinesOwed >= MAX_FINE) return false;
    return true;
}

void Member::addActiveBorrow(int transactionId) {
    m_activeBorrowIds.push_back(transactionId);
}

void Member::removeActiveBorrow(int transactionId) {
    m_activeBorrowIds.erase(
        std::remove(m_activeBorrowIds.begin(), m_activeBorrowIds.end(), transactionId),
        m_activeBorrowIds.end()
    );
}

int Member::getActiveBorrowCount() const {
    return static_cast<int>(m_activeBorrowIds.size());
}

void Member::addFine(double amount) {
    m_totalFinesOwed += amount;
}

void Member::clearFine(double amount) {
    m_totalFinesOwed -= amount;
    if (m_totalFinesOwed < 0.0) m_totalFinesOwed = 0.0;
}
