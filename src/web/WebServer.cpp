#include "WebServer.h"
#include "BookRoutes.h"
#include "MemberRoutes.h"
#include "TransactionRoutes.h"
#include "httplib.h"
#include <iostream>

void startServer(LibraryService& libraryService,
                 SearchService&  searchService,
                 int port)
{
    httplib::Server svr;

    // ── CORS headers (allow frontend on any origin during dev) ─
    svr.set_default_headers({
        {"Access-Control-Allow-Origin",  "*"},
        {"Access-Control-Allow-Headers", "Content-Type"},
        {"Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS"}
    });
    svr.Options(".*", [](const httplib::Request&, httplib::Response& res) {
        res.status = 204;
    });

    // ── Bypass Threading for MinGW GCC 6.3.0 ──────────────────
    class SyncQueue : public httplib::TaskQueue {
    public:
        void enqueue(std::function<void()> fn) override {
            fn(); // execute immediately in the same thread
        }
        void shutdown() override {}
    };
    svr.new_task_queue = [] { return new SyncQueue(); };

    // ── Register route groups ──────────────────────────────────
    registerBookRoutes(svr, libraryService, searchService);
    registerMemberRoutes(svr, libraryService);
    registerTransactionRoutes(svr, libraryService);

    // ── Serve static frontend from ./frontend/ ────────────────
    svr.set_mount_point("/", "./frontend");

    std::cout << "\n╔══════════════════════════════════════╗\n";
    std::cout << "║  Library System running on           ║\n";
    std::cout << "║  http://localhost:" << port << "            ║\n";
    std::cout << "╚══════════════════════════════════════╝\n\n";

    svr.listen("0.0.0.0", port);
}
