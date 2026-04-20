// ============================================================
//  main.cpp — Composition Root
// ============================================================
#include <iostream>
#include <cstdlib>   // getenv

#ifdef _WIN32
  #include <direct.h>
  #define MAKE_DIR(p) _mkdir(p)
#else
  #include <sys/stat.h>
  #define MAKE_DIR(p) mkdir(p, 0777)
#endif

#include "FileBookRepository.h"
#include "FileMemberRepository.h"
#include "FileTransactionRepository.h"
#include "LibraryService.h"
#include "SearchService.h"
#include "WebServer.h"

int main() {
    // ── Ensure data directory exists (cross-platform) ─────────
    MAKE_DIR("data");

    // ── Wire up repositories ──────────────────────────────────
    FileBookRepository        bookRepo("data/books.json");
    FileMemberRepository      memberRepo("data/members.json");
    FileTransactionRepository txRepo("data/transactions.json");

    // ── Inject repositories into service layer ────────────────
    LibraryService libraryService(bookRepo, memberRepo, txRepo);

    // ── SearchService ─────────────────────────────────────────
    SearchService searchService([&libraryService]() {
        return libraryService.getAllBooks();
    });

    // ── Port: read from $PORT env var (Render sets this) ─────
    int port = 8080;
    const char* envPort = std::getenv("PORT");
    if (envPort) port = std::atoi(envPort);

    // ── Start REST server ────────────────────────────────────
    startServer(libraryService, searchService, port);

    return 0;
}
