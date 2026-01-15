#include "Database.hpp"

DataBase::DataBase(const std::string &path)
{
    sqlite3_open(path.c_str(), &_db);

    if (!_db)
        throw Error("DataBase", "Impossible to read the DB. Maybe DB don't exist.");
}

DataBase::~DataBase()
{
    if (_db) {
        sqlite3_close(_db);
        _db = nullptr;
    }
}

void DataBase::exec(const std::string &sql)
{
    char *error = nullptr;
    const int exitCode = sqlite3_exec(_db, sql.c_str(), nullptr, nullptr, &error);

    if (exitCode != SQLITE_OK) {
        std::string msg;
        if (error == nullptr)
            msg = "Unknown sqlite error";
        else
            msg = error;

        sqlite3_free(error);
        throw Error("Database::exec", msg);
    }
}

sqlite3 *DataBase::handle() const
{
    return _db;
}
