#include "Book.h"

Book::Book(int id,
           const std::string& title,
           const std::string& author,
           const std::string& isbn,
           const std::string& category,
           int totalCopies)
    : m_id(id), m_title(title), m_author(author),
      m_isbn(isbn), m_category(category),
      m_status(BookStatus::AVAILABLE),
      m_totalCopies(totalCopies),
      m_availableCopies(totalCopies)
{}

void Book::updateStatus() {
    if (m_availableCopies > 0)
        m_status = BookStatus::AVAILABLE;
    else
        m_status = BookStatus::BORROWED;
}

bool Book::checkout() {
    if (m_availableCopies <= 0) return false;
    --m_availableCopies;
    updateStatus();
    return true;
}

bool Book::returnCopy() {
    if (m_availableCopies >= m_totalCopies) return false;
    ++m_availableCopies;
    updateStatus();
    return true;
}

std::string Book::statusToString(BookStatus s) {
    switch (s) {
        case BookStatus::AVAILABLE: return "AVAILABLE";
        case BookStatus::BORROWED:  return "BORROWED";
        case BookStatus::RESERVED:  return "RESERVED";
        case BookStatus::LOST:      return "LOST";
        default:                    return "UNKNOWN";
    }
}

BookStatus Book::stringToStatus(const std::string& s) {
    if (s == "BORROWED")  return BookStatus::BORROWED;
    if (s == "RESERVED")  return BookStatus::RESERVED;
    if (s == "LOST")      return BookStatus::LOST;
    return BookStatus::AVAILABLE;
}
