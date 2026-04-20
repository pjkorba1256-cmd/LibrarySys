#pragma once
#include "IRepository.h"
#include "Member.h"
#include <string>
#include <vector>

class FileMemberRepository : public IRepository<Member> {
private:
    std::string         m_filePath;
    std::vector<Member> m_cache;

    void loadFromDisk();
    void flushToDisk() const;

public:
    explicit FileMemberRepository(const std::string& filePath);

    Member              findById(int id)          override;
    std::vector<Member> findAll()                 override;
    void                save(const Member& member) override;
    void                remove(int id)            override;
    int                 nextId()                  override;
};
