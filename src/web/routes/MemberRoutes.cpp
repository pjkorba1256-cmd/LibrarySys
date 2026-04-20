#include "MemberRoutes.h"
#include "json.hpp"
#include <stdexcept>

using json = nlohmann::json;

static json memberToJson(const Member& m) {
    json borrows = json::array();
    for (int id : m.getActiveBorrowIds()) borrows.push_back(id);

    return {
        {"id",                m.getId()},
        {"name",              m.getName()},
        {"email",             m.getEmail()},
        {"role",              m.getRole()},
        {"finesOwed",         m.getTotalFinesOwed()},
        {"activeBorrowCount", m.getActiveBorrowCount()},
        {"activeBorrowIds",   borrows},
        {"canBorrow",         m.canBorrowBooks()}
    };
}

void registerMemberRoutes(httplib::Server& svr, LibraryService& service) {

    // GET /api/members
    svr.Get("/api/members", [&](const httplib::Request&, httplib::Response& res) {
        try {
            auto members = service.getAllMembers();
            json j       = json::array();
            for (const auto& m : members) j.push_back(memberToJson(m));
            res.set_content(j.dump(), "application/json");
        } catch (const std::exception& e) {
            res.status = 500;
            res.set_content(json{{"error", e.what()}}.dump(), "application/json");
        }
    });

    // GET /api/members/:id
    svr.Get(R"(/api/members/(\d+))", [&](const httplib::Request& req, httplib::Response& res) {
        try {
            int id  = std::stoi(req.matches[1]);
            auto m  = service.getMember(id);
            res.set_content(memberToJson(m).dump(), "application/json");
        } catch (const std::exception& e) {
            res.status = 404;
            res.set_content(json{{"error", e.what()}}.dump(), "application/json");
        }
    });

    // POST /api/members — register new member
    svr.Post("/api/members", [&](const httplib::Request& req, httplib::Response& res) {
        try {
            auto body   = json::parse(req.body);
            auto member = service.addMember(
                body["name"].get<std::string>(),
                body["email"].get<std::string>()
            );
            res.status = 201;
            res.set_content(memberToJson(member).dump(), "application/json");
        } catch (const std::exception& e) {
            res.status = 400;
            res.set_content(json{{"error", e.what()}}.dump(), "application/json");
        }
    });

    // DELETE /api/members/:id
    svr.Delete(R"(/api/members/(\d+))", [&](const httplib::Request& req, httplib::Response& res) {
        try {
            int id = std::stoi(req.matches[1]);
            service.deleteMember(id);
            res.set_content(json{{"success", true}}.dump(), "application/json");
        } catch (const std::exception& e) {
            res.status = 404;
            res.set_content(json{{"error", e.what()}}.dump(), "application/json");
        }
    });
}
