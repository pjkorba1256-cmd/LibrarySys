#pragma once
#include <string>
#include <vector>

// ============================================================
//  Book — core domain entity representing a library book.
//  Encapsulates copy management and status transitions.
// ============================================================
enum class BookStatus { AVAILABLE, BORROWED, RESERVED, LOST };

class Book {
private:
    int         m_id;
    std::string m_title;
    std::string m_author;
    std::string m_isbn;
    std::string m_category;
    BookStatus  m_status;
    int         m_totalCopies;
    int         m_availableCopies;

    void updateStatus();  // auto-updates status from availableCopies

public:
    Book(int id,
         const std::string& title,
         const std::string& author,
         const std::string& isbn,
         const std::string& category = "General",
         int totalCopies = 1);

    // ── Encapsulated state transitions ────────────────────────
    bool checkout();    // returns false if unavailable
    bool returnCopy();  // increments available count

    // ── Getters ───────────────────────────────────────────────
    int         getId()              const { return m_id; }
    std::string getTitle()           const { return m_title; }
    std::string getAuthor()          const { return m_author; }
    std::string getISBN()            const { return m_isbn; }
    std::string getCategory()        const { return m_category; }
    BookStatus  getStatus()          const { return m_status; }
    int         getTotalCopies()     const { return m_totalCopies; }
    int         getAvailableCopies() const { return m_availableCopies; }
    bool        isAvailable()        const { return m_availableCopies > 0; }

    // ── Setters (for persistence restore) ─────────────────────
    void setStatus(BookStatus s)           { m_status = s; }
    void setAvailableCopies(int n)         { m_availableCopies = n; }
    void setTotalCopies(int n)             { m_totalCopies = n; }
    void setTitle(const std::string& t)    { m_title = t; }
    void setAuthor(const std::string& a)   { m_author = a; }
    void setCategory(const std::string& c) { m_category = c; }

    // ── Utility ───────────────────────────────────────────────
    static std::string statusToString(BookStatus s);
    static BookStatus  stringToStatus(const std::string& s);
};
