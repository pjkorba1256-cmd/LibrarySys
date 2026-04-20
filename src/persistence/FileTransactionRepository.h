#pragma once
#include "IRepository.h"
#include "Transaction.h"
#include <string>
#include <vector>

class FileTransactionRepository : public IRepository<Transaction> {
private:
    std::string              m_filePath;
    std::vector<Transaction> m_cache;

    void loadFromDisk();
    void flushToDisk() const;

public:
    explicit FileTransactionRepository(const std::string& filePath);

    Transaction              findById(int id)                  override;
    std::vector<Transaction> findAll()                         override;
    void                     save(const Transaction& tx)       override;
    void                     remove(int id)                    override;
    int                      nextId()                          override;

    // Extra query: get active borrows for a member
    std::vector<Transaction> findActiveByMember(int memberId);
};
