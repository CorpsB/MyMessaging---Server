#pragma once

#include "../../Include.hpp"

class DataBase {
    public:
        explicit DataBase(const std::string &path);
        ~DataBase();

        DataBase(const DataBase&) = delete;
        DataBase& operator=(const DataBase&) = delete;

        sqlite3 *handle() const;
        void exec(const std::string &sql);
    
    private:
        sqlite3 *_db;
};
