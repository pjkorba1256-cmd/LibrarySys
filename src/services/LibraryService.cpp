#include "LibraryService.h"
#include <stdexcept>
#include <algorithm>

LibraryService::LibraryService(FileBookRepository& bookRepo,
                               FileMemberRepository& memberRepo,
                               FileTransactionRepository& txRepo)
    : m_bookRepo(bookRepo), m_memberRepo(memberRepo), m_txRepo(txRepo) {}

// ══════════════════════════════════════════════════════════════
//  BOOK CRUD
// ══════════════════════════════════════════════════════════════
std::vector<Book> LibraryService::getAllBooks() {
    return m_bookRepo.findAll();
}

Book LibraryService::getBook(int bookId) {
    return m_bookRepo.findById(bookId);
}

Book LibraryService::addBook(const std::string& title, const std::string& author,
                              const std::string& isbn,  const std::string& category,
                              int copies) {
    int id = m_bookRepo.nextId();
    Book book(id, title, author, isbn, category, copies);
    m_bookRepo.save(book);
    return book;
}

void LibraryService::updateBook(const Book& book) {
    m_bookRepo.save(book);
}

void LibraryService::deleteBook(int bookId) {
    m_bookRepo.remove(bookId);
}

// ══════════════════════════════════════════════════════════════
//  MEMBER CRUD
// ══════════════════════════════════════════════════════════════
std::vector<Member> LibraryService::getAllMembers() {
    return m_memberRepo.findAll();
}

Member LibraryService::getMember(int memberId) {
    return m_memberRepo.findById(memberId);
}

Member LibraryService::addMember(const std::string& name, const std::string& email) {
    int id = m_memberRepo.nextId();
    Member member(id, name, email);
    m_memberRepo.save(member);
    return member;
}

void LibraryService::deleteMember(int memberId) {
    m_memberRepo.remove(memberId);
}

// ══════════════════════════════════════════════════════════════
//  BORROW BOOK — core use case
// ══════════════════════════════════════════════════════════════
Result<Transaction> LibraryService::borrowBook(int memberId, int bookId) {
    Member member = m_memberRepo.findById(memberId);
    Book   book   = m_bookRepo.findById(bookId);

    // ── Guard: eligibility checks (encapsulated in domain) ────
    if (!member.canBorrowBooks())
        return Result<Transaction>::error(
            "Member cannot borrow: borrow limit reached or outstanding fines exceed ₹200"
        );
    if (!book.isAvailable())
        return Result<Transaction>::error("Book '" + book.getTitle() + "' is not available");

    // ── Domain operations ─────────────────────────────────────
    int txId = m_txRepo.nextId();
    Transaction tx(txId, bookId, memberId);

    book.checkout();                   // decrements available copies
    member.addActiveBorrow(txId);      // tracks in member state

    // ── Persist all three affected objects ────────────────────
    m_bookRepo.save(book);
    m_memberRepo.save(member);
    m_txRepo.save(tx);

    return Result<Transaction>::ok(tx);
}

// ══════════════════════════════════════════════════════════════
//  RETURN BOOK — fine is computed entirely inside Transaction
// ══════════════════════════════════════════════════════════════
Result<double> LibraryService::returnBook(int transactionId) {
    Transaction tx     = m_txRepo.findById(transactionId);
    Book        book   = m_bookRepo.findById(tx.getBookId());
    Member      member = m_memberRepo.findById(tx.getMemberId());

    if (tx.getStatus() == TransactionStatus::RETURNED)
        return Result<double>::error("Transaction already returned");

    // ── Encapsulation: Transaction owns fine logic ─────────────
    tx.markReturned();                    // sets returnDate, computes fine internally
    double fine = tx.getFineCharged();

    book.returnCopy();                    // increments available copies
    member.removeActiveBorrow(transactionId);
    if (fine > 0.0) member.addFine(fine); // charge member account

    // ── Persist ───────────────────────────────────────────────
    m_txRepo.save(tx);
    m_bookRepo.save(book);
    m_memberRepo.save(member);

    return Result<double>::ok(fine);
}

void LibraryService::markBookLost(int transactionId) {
    Transaction tx     = m_txRepo.findById(transactionId);
    Member      member = m_memberRepo.findById(tx.getMemberId());

    tx.markLost();
    member.removeActiveBorrow(transactionId);
    member.addFine(tx.getFineCharged());

    m_txRepo.save(tx);
    m_memberRepo.save(member);
}

// ══════════════════════════════════════════════════════════════
//  QUERIES
// ══════════════════════════════════════════════════════════════
std::vector<Transaction> LibraryService::getAllTransactions() {
    return m_txRepo.findAll();
}

std::vector<Transaction> LibraryService::getActiveTransactions() {
    auto all = m_txRepo.findAll();
    std::vector<Transaction> active;
    for (const auto& tx : all)
        if (tx.getStatus() == TransactionStatus::ACTIVE) active.push_back(tx);
    return active;
}

std::vector<Transaction> LibraryService::getMemberTransactions(int memberId) {
    auto all = m_txRepo.findAll();
    std::vector<Transaction> result;
    for (const auto& tx : all)
        if (tx.getMemberId() == memberId) result.push_back(tx);
    return result;
}
