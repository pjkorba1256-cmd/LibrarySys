#include "FileBookRepository.h"
#include "json.hpp"
#include <fstream>
#include <stdexcept>
#include <algorithm>

using json = nlohmann::json;

FileBookRepository::FileBookRepository(const std::string& filePath)
    : m_filePath(filePath)
{
    loadFromDisk();
}

void FileBookRepository::loadFromDisk() {
    m_cache.clear();
    std::ifstream file(m_filePath);
    if (!file.is_open()) return; // first run: empty store

    try {
        json j;
        file >> j;
        for (const auto& item : j) {
            Book b(
                item["id"].get<int>(),
                item["title"].get<std::string>(),
                item["author"].get<std::string>(),
                item["isbn"].get<std::string>(),
                item.value("category", "General"),
                item.value("totalCopies", 1)
            );
            b.setAvailableCopies(item.value("availableCopies", 1));
            b.setStatus(Book::stringToStatus(item.value("status", "AVAILABLE")));
            m_cache.push_back(b);
        }
    } catch (...) {
        m_cache.clear(); // corrupt file: start fresh
    }
}

void FileBookRepository::flushToDisk() const {
    json j = json::array();
    for (const auto& b : m_cache) {
        j.push_back({
            {"id",              b.getId()},
            {"title",           b.getTitle()},
            {"author",          b.getAuthor()},
            {"isbn",            b.getISBN()},
            {"category",        b.getCategory()},
            {"status",          Book::statusToString(b.getStatus())},
            {"totalCopies",     b.getTotalCopies()},
            {"availableCopies", b.getAvailableCopies()}
        });
    }
    std::ofstream file(m_filePath);
    file << j.dump(2);
}

Book FileBookRepository::findById(int id) {
    for (const auto& b : m_cache)
        if (b.getId() == id) return b;
    throw std::runtime_error("Book not found: id=" + std::to_string(id));
}

std::vector<Book> FileBookRepository::findAll() {
    return m_cache;
}

void FileBookRepository::save(const Book& book) {
    for (auto& b : m_cache) {
        if (b.getId() == book.getId()) {
            b = book;
            flushToDisk();
            return;
        }
    }
    m_cache.push_back(book);
    flushToDisk();
}

void FileBookRepository::remove(int id) {
    m_cache.erase(
        std::remove_if(m_cache.begin(), m_cache.end(),
            [id](const Book& b) { return b.getId() == id; }),
        m_cache.end()
    );
    flushToDisk();
}

int FileBookRepository::nextId() {
    int maxId = 0;
    for (const auto& b : m_cache)
        if (b.getId() > maxId) maxId = b.getId();
    return maxId + 1;
}
