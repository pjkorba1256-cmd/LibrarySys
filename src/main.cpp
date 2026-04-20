// ============================================================
//  main.cpp — Composition Root
//  This is the ONLY file that chooses concrete implementations.
//  All other code depends on interfaces.
// ============================================================
#include <iostream>
#include <direct.h>    // _mkdir — Windows/MinGW alternative to std::filesystem

#include "FileBookRepository.h"
#include "FileMemberRepository.h"
#include "FileTransactionRepository.h"
#include "LibraryService.h"
#include "SearchService.h"
#include "WebServer.h"

int main() {
    // ── Ensure data directory exists (MinGW-compatible) ────────
    _mkdir("data");

    // ── Wire up repositories (swap here for SQLite — nothing else changes) ──
    FileBookRepository        bookRepo("data/books.json");
    FileMemberRepository      memberRepo("data/members.json");
    FileTransactionRepository txRepo("data/transactions.json");

    // ── Inject repositories into service layer ────────────────
    LibraryService libraryService(bookRepo, memberRepo, txRepo);

    // ── SearchService: reads live book list via lambda ─────────
    SearchService searchService([&libraryService]() {
        return libraryService.getAllBooks();
    });

    // ── Start REST server (serves frontend + API on :8080) ────
    startServer(libraryService, searchService, 8080);

    return 0;
}
