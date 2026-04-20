#pragma once
#include "FileBookRepository.h"
#include "FileMemberRepository.h"
#include "FileTransactionRepository.h"
#include "Book.h"
#include "Member.h"
#include "Transaction.h"
#include <string>
#include <vector>

// ============================================================
//  Result<T> — lightweight Either type for error handling.
//  Avoids exceptions in service-level return paths.
// ============================================================
template<typename T>
class Result {
private:
    bool        m_ok;
    T           m_value;
    std::string m_error;

    Result() = default;

public:
    static Result<T> ok(const T& value) {
        Result<T> r;
        r.m_ok    = true;
        r.m_value = value;
        return r;
    }
    static Result<T> error(const std::string& msg) {
        Result<T> r;
        r.m_ok    = false;
        r.m_error = msg;
        return r;
    }

    bool        isOk()       const { return m_ok; }
    T           getValue()   const { return m_value; }
    std::string getError()   const { return m_error; }
};

// ============================================================
//  LibraryService — Orchestrates all borrow/return use cases.
//  Injected with repository interfaces; knows nothing of HTTP.
// ============================================================
class LibraryService {
private:
    FileBookRepository&        m_bookRepo;
    FileMemberRepository&      m_memberRepo;
    FileTransactionRepository& m_txRepo;

public:
    LibraryService(FileBookRepository& bookRepo,
                   FileMemberRepository& memberRepo,
                   FileTransactionRepository& txRepo);

    // ── Book CRUD ─────────────────────────────────────────────
    std::vector<Book> getAllBooks();
    Book              getBook(int bookId);
    Book              addBook(const std::string& title, const std::string& author,
                              const std::string& isbn,  const std::string& category,
                              int copies);
    void              updateBook(const Book& book);
    void              deleteBook(int bookId);

    // ── Member CRUD ───────────────────────────────────────────
    std::vector<Member> getAllMembers();
    Member              getMember(int memberId);
    Member              addMember(const std::string& name, const std::string& email);
    void                deleteMember(int memberId);

    // ── Core Borrow/Return Use Cases ──────────────────────────
    Result<Transaction> borrowBook(int memberId, int bookId);
    Result<double>      returnBook(int transactionId);
    void                markBookLost(int transactionId);

    // ── Queries ───────────────────────────────────────────────
    std::vector<Transaction> getAllTransactions();
    std::vector<Transaction> getActiveTransactions();
    std::vector<Transaction> getMemberTransactions(int memberId);
};
