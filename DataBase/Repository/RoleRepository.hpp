#pragma once

#include "../../Include.hpp"

class DataBase;

class RoleRepository {
  public:
    explicit RoleRepository(DataBase &db);

    int roleId(Role_t role) const;

    Role_t roleFromId(int roleId) const;

  private:
    DataBase &_db;
};
