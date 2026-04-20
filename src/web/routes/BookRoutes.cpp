#include "BookRoutes.h"
#include "json.hpp"
#include <stdexcept>

using json = nlohmann::json;

// ── Serialise a Book to JSON ──────────────────────────────────
static json bookToJson(const Book& b) {
    return {
        {"id",              b.getId()},
        {"title",           b.getTitle()},
        {"author",          b.getAuthor()},
        {"isbn",            b.getISBN()},
        {"category",        b.getCategory()},
        {"status",          Book::statusToString(b.getStatus())},
        {"totalCopies",     b.getTotalCopies()},
        {"availableCopies", b.getAvailableCopies()}
    };
}

void registerBookRoutes(httplib::Server& svr,
                        LibraryService&  service,
                        SearchService&   search)
{
    // GET /api/books — list all
    svr.Get("/api/books", [&](const httplib::Request& req, httplib::Response& res) {
        try {
            // If ?q= present, search instead
            if (req.has_param("q")) {
                std::string q = req.get_param_value("q");
                auto results  = search.searchAll(q);
                json j        = json::array();
                for (const auto& b : results) j.push_back(bookToJson(b));
                res.set_content(j.dump(), "application/json");
            } else {
                auto books = service.getAllBooks();
                json j     = json::array();
                for (const auto& b : books) j.push_back(bookToJson(b));
                res.set_content(j.dump(), "application/json");
            }
        } catch (const std::exception& e) {
            res.status = 500;
            res.set_content(json{{"error", e.what()}}.dump(), "application/json");
        }
    });

    // GET /api/books/:id — single book
    svr.Get(R"(/api/books/(\d+))", [&](const httplib::Request& req, httplib::Response& res) {
        try {
            int id  = std::stoi(req.matches[1]);
            auto b  = service.getBook(id);
            res.set_content(bookToJson(b).dump(), "application/json");
        } catch (const std::exception& e) {
            res.status = 404;
            res.set_content(json{{"error", e.what()}}.dump(), "application/json");
        }
    });

    // POST /api/books — add new book
    svr.Post("/api/books", [&](const httplib::Request& req, httplib::Response& res) {
        try {
            auto body    = json::parse(req.body);
            auto book    = service.addBook(
                body["title"].get<std::string>(),
                body["author"].get<std::string>(),
                body.value("isbn", ""),
                body.value("category", "General"),
                body.value("copies", 1)
            );
            res.status = 201;
            res.set_content(bookToJson(book).dump(), "application/json");
        } catch (const std::exception& e) {
            res.status = 400;
            res.set_content(json{{"error", e.what()}}.dump(), "application/json");
        }
    });

    // DELETE /api/books/:id
    svr.Delete(R"(/api/books/(\d+))", [&](const httplib::Request& req, httplib::Response& res) {
        try {
            int id = std::stoi(req.matches[1]);
            service.deleteBook(id);
            res.set_content(json{{"success", true}}.dump(), "application/json");
        } catch (const std::exception& e) {
            res.status = 404;
            res.set_content(json{{"error", e.what()}}.dump(), "application/json");
        }
    });
}
