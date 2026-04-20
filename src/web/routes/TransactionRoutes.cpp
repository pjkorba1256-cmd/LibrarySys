#include "TransactionRoutes.h"
#include "json.hpp"
#include <stdexcept>

using json = nlohmann::json;

static json txToJson(const Transaction& tx) {
    return {
        {"id",           tx.getId()},
        {"bookId",       tx.getBookId()},
        {"memberId",     tx.getMemberId()},
        {"borrowDate",   Transaction::timeToString(tx.getBorrowDate())},
        {"dueDate",      Transaction::timeToString(tx.getDueDate())},
        {"returnDate",   Transaction::timeToString(tx.getReturnDate())},
        {"status",       Transaction::statusToString(tx.getStatus())},
        {"fineCharged",  tx.getFineCharged()},
        {"currentFine",  tx.computeFine()},
        {"isOverdue",    tx.isOverdue()}
    };
}

void registerTransactionRoutes(httplib::Server& svr, LibraryService& service) {

    // GET /api/transactions — all
    svr.Get("/api/transactions", [&](const httplib::Request& req, httplib::Response& res) {
        try {
            std::vector<Transaction> txs;
            if (req.has_param("memberId")) {
                txs = service.getMemberTransactions(std::stoi(req.get_param_value("memberId")));
            } else if (req.has_param("active") && req.get_param_value("active") == "true") {
                txs = service.getActiveTransactions();
            } else {
                txs = service.getAllTransactions();
            }
            json j = json::array();
            for (const auto& tx : txs) j.push_back(txToJson(tx));
            res.set_content(j.dump(), "application/json");
        } catch (const std::exception& e) {
            res.status = 500;
            res.set_content(json{{"error", e.what()}}.dump(), "application/json");
        }
    });

    // POST /api/borrow — borrow a book
    svr.Post("/api/borrow", [&](const httplib::Request& req, httplib::Response& res) {
        try {
            auto body     = json::parse(req.body);
            int memberId  = body["memberId"].get<int>();
            int bookId    = body["bookId"].get<int>();

            auto result = service.borrowBook(memberId, bookId);
            if (result.isOk()) {
                res.status = 201;
                res.set_content(txToJson(result.getValue()).dump(), "application/json");
            } else {
                res.status = 400;
                res.set_content(json{{"error", result.getError()}}.dump(), "application/json");
            }
        } catch (const std::exception& e) {
            res.status = 400;
            res.set_content(json{{"error", e.what()}}.dump(), "application/json");
        }
    });

    // POST /api/return — return a book
    svr.Post("/api/return", [&](const httplib::Request& req, httplib::Response& res) {
        try {
            auto body          = json::parse(req.body);
            int transactionId  = body["transactionId"].get<int>();

            auto result = service.returnBook(transactionId);
            if (result.isOk()) {
                res.set_content(
                    json{{"success", true}, {"fine", result.getValue()}}.dump(),
                    "application/json"
                );
            } else {
                res.status = 400;
                res.set_content(json{{"error", result.getError()}}.dump(), "application/json");
            }
        } catch (const std::exception& e) {
            res.status = 400;
            res.set_content(json{{"error", e.what()}}.dump(), "application/json");
        }
    });

    // POST /api/lost — mark book as lost
    svr.Post("/api/lost", [&](const httplib::Request& req, httplib::Response& res) {
        try {
            auto body         = json::parse(req.body);
            int transactionId = body["transactionId"].get<int>();
            service.markBookLost(transactionId);
            res.set_content(json{{"success", true}}.dump(), "application/json");
        } catch (const std::exception& e) {
            res.status = 400;
            res.set_content(json{{"error", e.what()}}.dump(), "application/json");
        }
    });
}
