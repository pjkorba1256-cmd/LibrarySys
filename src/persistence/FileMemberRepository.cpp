#include "FileMemberRepository.h"
#include "json.hpp"
#include <fstream>
#include <stdexcept>
#include <algorithm>

using json = nlohmann::json;

FileMemberRepository::FileMemberRepository(const std::string& filePath)
    : m_filePath(filePath)
{
    loadFromDisk();
}

void FileMemberRepository::loadFromDisk() {
    m_cache.clear();
    std::ifstream file(m_filePath);
    if (!file.is_open()) return;

    try {
        json j;
        file >> j;
        for (const auto& item : j) {
            Member m(
                item["id"].get<int>(),
                item["name"].get<std::string>(),
                item["email"].get<std::string>()
            );
            m.setFinesOwed(item.value("finesOwed", 0.0));
            if (item.contains("activeBorrows")) {
                std::vector<int> borrows = item["activeBorrows"].get<std::vector<int>>();
                m.setActiveBorrows(borrows);
            }
            m_cache.push_back(m);
        }
    } catch (...) {
        m_cache.clear();
    }
}

void FileMemberRepository::flushToDisk() const {
    json j = json::array();
    for (const auto& m : m_cache) {
        j.push_back({
            {"id",            m.getId()},
            {"name",          m.getName()},
            {"email",         m.getEmail()},
            {"finesOwed",     m.getTotalFinesOwed()},
            {"activeBorrows", m.getActiveBorrowIds()}
        });
    }
    std::ofstream file(m_filePath);
    file << j.dump(2);
}

Member FileMemberRepository::findById(int id) {
    for (const auto& m : m_cache)
        if (m.getId() == id) return m;
    throw std::runtime_error("Member not found: id=" + std::to_string(id));
}

std::vector<Member> FileMemberRepository::findAll() {
    return m_cache;
}

void FileMemberRepository::save(const Member& member) {
    for (auto& m : m_cache) {
        if (m.getId() == member.getId()) {
            m = member;
            flushToDisk();
            return;
        }
    }
    m_cache.push_back(member);
    flushToDisk();
}

void FileMemberRepository::remove(int id) {
    m_cache.erase(
        std::remove_if(m_cache.begin(), m_cache.end(),
            [id](const Member& m) { return m.getId() == id; }),
        m_cache.end()
    );
    flushToDisk();
}

int FileMemberRepository::nextId() {
    int maxId = 0;
    for (const auto& m : m_cache)
        if (m.getId() > maxId) maxId = m.getId();
    return maxId + 1;
}
