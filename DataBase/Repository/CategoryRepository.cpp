#include "CategoryRepository.hpp"

#include "Database.hpp"

static void checkPrepare(int rc, sqlite3 *handle, const char *where)
{
    if (rc != SQLITE_OK)
        throw Error(where, std::string(sqlite3_errmsg(handle)));
}

static void checkStepDone(int rc, sqlite3 *handle, const char *where)
{
    if (rc != SQLITE_DONE)
        throw Error(where, std::string(sqlite3_errmsg(handle)));
}

static void checkStepRow(int rc, sqlite3 *handle, const char *where)
{
    if (rc != SQLITE_ROW) {
        if (rc == SQLITE_DONE)
            throw Error(where, "Not found");
        throw Error(where, std::string(sqlite3_errmsg(handle)));
    }
}

CategoryRepository::CategoryRepository(DataBase &db) : _db(db) {}

categories_s CategoryRepository::rowToCategory(sqlite3_stmt *stmt) const
{
    categories_s c;
    c.CategoryID = static_cast<unsigned int>(sqlite3_column_int(stmt, 0));

    const unsigned char *name = sqlite3_column_text(stmt, 1);
    c.CategoryName = name ? reinterpret_cast<const char *>(name) : "";

    c.Position = static_cast<unsigned int>(sqlite3_column_int(stmt, 2));
    return c;
}

int CategoryRepository::createCategory(const std::string &name, unsigned int position)
{
    const char *sql = "INSERT INTO Categories (CategoryID, CategoryName, Position) VALUES (NULL, ?, ?)";
    sqlite3_stmt *stmt = nullptr;

    int rc = sqlite3_prepare_v2(_db.handle(), sql, -1, &stmt, nullptr);
    checkPrepare(rc, _db.handle(), "CategoryRepository::createCategory/prepare");

    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, static_cast<int>(position));

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    checkStepDone(rc, _db.handle(), "CategoryRepository::createCategory/step");

    return static_cast<int>(sqlite3_last_insert_rowid(_db.handle()));
}

categories_s CategoryRepository::getById(unsigned int categoryId) const
{
    const char *sql = "SELECT CategoryID, CategoryName, Position FROM Categories WHERE CategoryID = ?";
    sqlite3_stmt *stmt = nullptr;

    int rc = sqlite3_prepare_v2(_db.handle(), sql, -1, &stmt, nullptr);
    checkPrepare(rc, _db.handle(), "CategoryRepository::getById/prepare");

    sqlite3_bind_int(stmt, 1, static_cast<int>(categoryId));

    rc = sqlite3_step(stmt);
    checkStepRow(rc, _db.handle(), "CategoryRepository::getById/step");

    categories_s c = rowToCategory(stmt);
    sqlite3_finalize(stmt);
    return c;
}

std::vector<categories_s> CategoryRepository::listAll() const
{
    const char *sql = "SELECT CategoryID, CategoryName, Position FROM Categories ORDER BY Position ASC";
    sqlite3_stmt *stmt = nullptr;

    int rc = sqlite3_prepare_v2(_db.handle(), sql, -1, &stmt, nullptr);
    checkPrepare(rc, _db.handle(), "CategoryRepository::listAll/prepare");

    std::vector<categories_s> out;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        out.push_back(rowToCategory(stmt));
    }
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE)
        throw Error("CategoryRepository::listAll/step", std::string(sqlite3_errmsg(_db.handle())));

    return out;
}

void CategoryRepository::deleteById(unsigned int categoryId)
{
    const char *sql = "DELETE FROM Categories WHERE CategoryID = ?";
    sqlite3_stmt *stmt = nullptr;

    int rc = sqlite3_prepare_v2(_db.handle(), sql, -1, &stmt, nullptr);
    checkPrepare(rc, _db.handle(), "CategoryRepository::deleteById/prepare");

    sqlite3_bind_int(stmt, 1, static_cast<int>(categoryId));

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    checkStepDone(rc, _db.handle(), "CategoryRepository::deleteById/step");
}
