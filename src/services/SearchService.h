#pragma once
#include "IBookSearch.h"
#include "Book.h"
#include <vector>
#include <string>
#include <functional>

// ============================================================
//  SearchService — implements IBookSearch.
//  Case-insensitive in-memory search over the book collection.
// ============================================================
class SearchService : public IBookSearch {
private:
    std::function<std::vector<Book>()> m_bookProvider; // injected supplier

    static std::string toLower(const std::string& s);
    std::vector<Book>  filterBooks(std::function<bool(const Book&)> pred) const;

public:
    // Takes a callable that returns the current book list (stays up to date)
    explicit SearchService(std::function<std::vector<Book>()> bookProvider);

    std::vector<Book> searchByTitle(const std::string& title)    const override;
    std::vector<Book> searchByAuthor(const std::string& author)  const override;
    std::vector<Book> searchByISBN(const std::string& isbn)      const override;
    std::vector<Book> searchByCategory(const std::string& cat)   const override;

    // Convenience: search across all fields at once
    std::vector<Book> searchAll(const std::string& query) const;
};
