#include "UserRepository.hpp"

UserRepository::UserRepository(DataBase &db, const RoleRepository &roles)
    : _db(db), _roles(roles)
{
}

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

int UserRepository::createUser(const std::string &userName, Role_t role)
{
    const char *sql = "INSERT INTO Users (UserName, UserRole) VALUES (?, ?)";
    sqlite3_stmt *stmt = nullptr;

    int rc = sqlite3_prepare_v2(_db.handle(), sql, -1, &stmt, nullptr);
    checkPrepare(rc, _db.handle(), "UserRepository::createUser/prepare");

    sqlite3_bind_text(stmt, 1, userName.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, _roles.roleId(role));

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    checkStepDone(rc, _db.handle(), "UserRepository::createUser/step");

    return static_cast<int>(sqlite3_last_insert_rowid(_db.handle()));
}

user UserRepository::rowToUser(sqlite3_stmt *stmt) const
{
    user u;

    u.UserID = sqlite3_column_int(stmt, 0);
    u.IsConnected = sqlite3_column_int(stmt, 1) != 0;

    const unsigned char *name = sqlite3_column_text(stmt, 2);
    u.UserName = name ? reinterpret_cast<const char *>(name) : "";

    const int roleId = sqlite3_column_int(stmt, 3);
    u.role = _roles.roleFromId(roleId);

    return u;
}

user UserRepository::getById(int userId) const
{
    const char *sql = "SELECT UserID, IsConnected, UserName, UserRole FROM Users WHERE UserID = ?";
    sqlite3_stmt *stmt = nullptr;

    int rc = sqlite3_prepare_v2(_db.handle(), sql, -1, &stmt, nullptr);
    checkPrepare(rc, _db.handle(), "UserRepository::getById/prepare");

    sqlite3_bind_int(stmt, 1, userId);

    rc = sqlite3_step(stmt);
    checkStepRow(rc, _db.handle(), "UserRepository::getById/step");

    user u = rowToUser(stmt);
    sqlite3_finalize(stmt);
    return u;
}

user UserRepository::getByName(const std::string &userName) const
{
    const char *sql = "SELECT UserID, IsConnected, UserName, UserRole FROM Users WHERE UserName = ?";
    sqlite3_stmt *stmt = nullptr;

    int rc = sqlite3_prepare_v2(_db.handle(), sql, -1, &stmt, nullptr);
    checkPrepare(rc, _db.handle(), "UserRepository::getByName/prepare");

    sqlite3_bind_text(stmt, 1, userName.c_str(), -1, SQLITE_TRANSIENT);

    rc = sqlite3_step(stmt);
    checkStepRow(rc, _db.handle(), "UserRepository::getByName/step");

    user u = rowToUser(stmt);
    sqlite3_finalize(stmt);
    return u;
}

void UserRepository::setConnected(int userId, bool connected)
{
    const char *sql = "UPDATE Users SET IsConnected = ? WHERE UserID = ?";
    sqlite3_stmt *stmt = nullptr;

    int rc = sqlite3_prepare_v2(_db.handle(), sql, -1, &stmt, nullptr);
    checkPrepare(rc, _db.handle(), "UserRepository::setConnected/prepare");

    sqlite3_bind_int(stmt, 1, connected ? 1 : 0);
    sqlite3_bind_int(stmt, 2, userId);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    checkStepDone(rc, _db.handle(), "UserRepository::setConnected/step");
}

std::vector<user> UserRepository::listAll() const
{
    const char *sql = "SELECT UserID, IsConnected, UserName, UserRole FROM Users ORDER BY UserID ASC";
    sqlite3_stmt *stmt = nullptr;

    int rc = sqlite3_prepare_v2(_db.handle(), sql, -1, &stmt, nullptr);
    checkPrepare(rc, _db.handle(), "UserRepository::listAll/prepare");

    std::vector<user> out;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        out.push_back(rowToUser(stmt));
    }
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE)
        throw Error("UserRepository::listAll/step", std::string(sqlite3_errmsg(_db.handle())));

    return out;
}
