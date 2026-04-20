#pragma once
#include "IRepository.h"
#include "Book.h"
#include <string>
#include <vector>

// ============================================================
//  FileBookRepository — Persists Book objects as JSON on disk.
//  Uses an in-memory cache; flushes on every write.
// ============================================================
class FileBookRepository : public IRepository<Book> {
private:
    std::string       m_filePath;
    std::vector<Book> m_cache;

    void loadFromDisk();
    void flushToDisk() const;

public:
    explicit FileBookRepository(const std::string& filePath);

    Book              findById(int id)        override;
    std::vector<Book> findAll()               override;
    void              save(const Book& book)  override;
    void              remove(int id)          override;
    int               nextId()               override;
};
