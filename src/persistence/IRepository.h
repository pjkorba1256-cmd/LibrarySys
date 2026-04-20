#pragma once
#include <vector>

// ============================================================
//  IRepository<T> — Generic repository interface.
//  Domain and service layers depend only on this abstraction;
//  concrete implementations (File / SQLite) are swappable.
// ============================================================
template<typename T>
class IRepository {
public:
    virtual ~IRepository() = default;

    virtual T              findById(int id)       = 0;
    virtual std::vector<T> findAll()              = 0;
    virtual void           save(const T& entity)  = 0;  // insert or update
    virtual void           remove(int id)         = 0;
    virtual int            nextId()               = 0;  // auto-increment helper
};
