#pragma once

#include "../../Include.hpp"

class DataBase;

class CategoryRepository {
  public:
    explicit CategoryRepository(DataBase &db);

    int createCategory(const std::string &name, unsigned int position);
    categories_s getById(unsigned int categoryId) const;
    std::vector<categories_s> listAll() const;
    void deleteById(unsigned int categoryId);

  private:
    categories_s rowToCategory(sqlite3_stmt *stmt) const;
    DataBase &_db;
};
