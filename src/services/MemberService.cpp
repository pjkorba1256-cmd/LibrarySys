#include "MemberService.h"

MemberService::MemberService(LibraryService& libraryService)
    : m_libraryService(libraryService) {}

std::vector<Transaction> MemberService::getMemberHistory(int memberId) {
    return m_libraryService.getMemberTransactions(memberId);
}

double MemberService::getMemberFines(int memberId) {
    Member m = m_libraryService.getMember(memberId);
    return m.getTotalFinesOwed();
}

bool MemberService::payFine(int /*memberId*/, double /*amount*/) {
    // Stub: payFine requires direct access to FileMemberRepository.
    // Wire this up by injecting the repo directly when needed.
    return false;
}
