#pragma once
#include "LibraryService.h"
#include "SearchService.h"

// ============================================================
//  MemberService — handles member-specific queries & actions.
// ============================================================
class MemberService {
private:
    LibraryService& m_libraryService;

public:
    explicit MemberService(LibraryService& libraryService);

    std::vector<Transaction> getMemberHistory(int memberId);
    double                   getMemberFines(int memberId);
    bool                     payFine(int memberId, double amount);
};
