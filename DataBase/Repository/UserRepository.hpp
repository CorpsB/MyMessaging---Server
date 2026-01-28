#pragma once

#include "../../Include.hpp"

class DataBase;
class RoleRepository;

class UserRepository {
  public:
    explicit UserRepository(DataBase &db, const RoleRepository &roles);

    int createUser(const std::string &userName, Role_t role = USER);

    user getById(int userId) const;
    user getByName(const std::string &userName) const;

    void setConnected(int userId, bool connected);

    std::vector<user> listAll() const;

  private:
    user rowToUser(sqlite3_stmt *stmt) const;

    DataBase &_db;
    const RoleRepository &_roles;
};
