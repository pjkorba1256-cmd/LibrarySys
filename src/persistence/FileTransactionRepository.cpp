#include "FileTransactionRepository.h"
#include "json.hpp"
#include <fstream>
#include <stdexcept>
#include <algorithm>

using json = nlohmann::json;

FileTransactionRepository::FileTransactionRepository(const std::string& filePath)
    : m_filePath(filePath)
{
    loadFromDisk();
}

void FileTransactionRepository::loadFromDisk() {
    m_cache.clear();
    std::ifstream file(m_filePath);
    if (!file.is_open()) return;

    try {
        json j;
        file >> j;
        for (const auto& item : j) {
            Transaction tx;
            tx.setId(item["id"].get<int>());
            tx.setBookId(item["bookId"].get<int>());
            tx.setMemberId(item["memberId"].get<int>());
            tx.setBorrowDate(Transaction::stringToTime(item.value("borrowDate", "0")));
            tx.setDueDate(Transaction::stringToTime(item.value("dueDate", "0")));
            tx.setReturnDate(Transaction::stringToTime(item.value("returnDate", "0")));
            tx.setStatus(Transaction::stringToStatus(item.value("status", "ACTIVE")));
            tx.setFineCharged(item.value("fineCharged", 0.0));
            m_cache.push_back(tx);
        }
    } catch (...) {
        m_cache.clear();
    }
}

void FileTransactionRepository::flushToDisk() const {
    json j = json::array();
    for (const auto& tx : m_cache) {
        j.push_back({
            {"id",          tx.getId()},
            {"bookId",      tx.getBookId()},
            {"memberId",    tx.getMemberId()},
            {"borrowDate",  Transaction::timeToString(tx.getBorrowDate())},
            {"dueDate",     Transaction::timeToString(tx.getDueDate())},
            {"returnDate",  Transaction::timeToString(tx.getReturnDate())},
            {"status",      Transaction::statusToString(tx.getStatus())},
            {"fineCharged", tx.getFineCharged()}
        });
    }
    std::ofstream file(m_filePath);
    file << j.dump(2);
}

Transaction FileTransactionRepository::findById(int id) {
    for (const auto& tx : m_cache)
        if (tx.getId() == id) return tx;
    throw std::runtime_error("Transaction not found: id=" + std::to_string(id));
}

std::vector<Transaction> FileTransactionRepository::findAll() {
    return m_cache;
}

void FileTransactionRepository::save(const Transaction& tx) {
    for (auto& t : m_cache) {
        if (t.getId() == tx.getId()) {
            t = tx;
            flushToDisk();
            return;
        }
    }
    m_cache.push_back(tx);
    flushToDisk();
}

void FileTransactionRepository::remove(int id) {
    m_cache.erase(
        std::remove_if(m_cache.begin(), m_cache.end(),
            [id](const Transaction& t) { return t.getId() == id; }),
        m_cache.end()
    );
    flushToDisk();
}

int FileTransactionRepository::nextId() {
    int maxId = 0;
    for (const auto& tx : m_cache)
        if (tx.getId() > maxId) maxId = tx.getId();
    return maxId + 1;
}

std::vector<Transaction> FileTransactionRepository::findActiveByMember(int memberId) {
    std::vector<Transaction> result;
    for (const auto& tx : m_cache)
        if (tx.getMemberId() == memberId && tx.getStatus() == TransactionStatus::ACTIVE)
            result.push_back(tx);
    return result;
}
