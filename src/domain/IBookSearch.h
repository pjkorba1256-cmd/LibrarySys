#pragma once
#include <vector>
#include <string>

// Forward declaration
class Book;

// ============================================================
//  IBookSearch — Search interface (Dependency Inversion).
//  SearchService implements this; callers depend only on this
//  interface, not on any concrete search implementation.
// ============================================================
class IBookSearch {
public:
    virtual ~IBookSearch() = default;

    virtual std::vector<Book> searchByTitle(const std::string& title)    const = 0;
    virtual std::vector<Book> searchByAuthor(const std::string& author)  const = 0;
    virtual std::vector<Book> searchByISBN(const std::string& isbn)      const = 0;
    virtual std::vector<Book> searchByCategory(const std::string& cat)   const = 0;
};
