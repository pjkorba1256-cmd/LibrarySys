#include "SearchService.h"
#include <algorithm>
#include <cctype>

SearchService::SearchService(std::function<std::vector<Book>()> bookProvider)
    : m_bookProvider(std::move(bookProvider)) {}

std::string SearchService::toLower(const std::string& s) {
    std::string result = s;
    std::transform(result.begin(), result.end(), result.begin(),
        [](unsigned char c) { return std::tolower(c); });
    return result;
}

std::vector<Book> SearchService::filterBooks(std::function<bool(const Book&)> pred) const {
    auto books = m_bookProvider();
    std::vector<Book> result;
    for (const auto& b : books)
        if (pred(b)) result.push_back(b);
    return result;
}

std::vector<Book> SearchService::searchByTitle(const std::string& title) const {
    std::string q = toLower(title);
    return filterBooks([&](const Book& b) {
        return toLower(b.getTitle()).find(q) != std::string::npos;
    });
}

std::vector<Book> SearchService::searchByAuthor(const std::string& author) const {
    std::string q = toLower(author);
    return filterBooks([&](const Book& b) {
        return toLower(b.getAuthor()).find(q) != std::string::npos;
    });
}

std::vector<Book> SearchService::searchByISBN(const std::string& isbn) const {
    return filterBooks([&](const Book& b) {
        return b.getISBN() == isbn;
    });
}

std::vector<Book> SearchService::searchByCategory(const std::string& cat) const {
    std::string q = toLower(cat);
    return filterBooks([&](const Book& b) {
        return toLower(b.getCategory()).find(q) != std::string::npos;
    });
}

std::vector<Book> SearchService::searchAll(const std::string& query) const {
    std::string q = toLower(query);
    return filterBooks([&](const Book& b) {
        return toLower(b.getTitle()).find(q)    != std::string::npos ||
               toLower(b.getAuthor()).find(q)   != std::string::npos ||
               toLower(b.getCategory()).find(q) != std::string::npos ||
               b.getISBN().find(query)          != std::string::npos;
    });
}
