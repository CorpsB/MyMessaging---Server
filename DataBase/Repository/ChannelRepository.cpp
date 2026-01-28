#include "ChannelRepository.hpp"

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

ChannelRepository::ChannelRepository(DataBase &db) : _db(db) {}

channels ChannelRepository::rowToChannel(sqlite3_stmt *stmt) const
{
    channels ch;
    ch.ChannelID = static_cast<unsigned int>(sqlite3_column_int(stmt, 0));
    ch.Position  = static_cast<unsigned int>(sqlite3_column_int(stmt, 1));

    const unsigned char *name = sqlite3_column_text(stmt, 2);
    ch.ChannelName = name ? reinterpret_cast<const char *>(name) : "";

    ch.CategoryID = static_cast<unsigned int>(sqlite3_column_int(stmt, 3));
    return ch;
}

int ChannelRepository::createChannel(const std::string &name, unsigned int position, unsigned int categoryId)
{
    const char *sql = "INSERT INTO Channels (ChannelID, Position, SalonName, CategoryID) VALUES (NULL, ?, ?, ?)";
    sqlite3_stmt *stmt = nullptr;

    int rc = sqlite3_prepare_v2(_db.handle(), sql, -1, &stmt, nullptr);
    checkPrepare(rc, _db.handle(), "ChannelRepository::createChannel/prepare");

    sqlite3_bind_int(stmt, 1, static_cast<int>(position));
    sqlite3_bind_text(stmt, 2, name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 3, static_cast<int>(categoryId));

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    checkStepDone(rc, _db.handle(), "ChannelRepository::createChannel/step");

    return static_cast<int>(sqlite3_last_insert_rowid(_db.handle()));
}

channels ChannelRepository::getById(unsigned int channelId) const
{
    const char *sql = "SELECT ChannelID, Position, SalonName, CategoryID FROM Channels WHERE ChannelID = ?";
    sqlite3_stmt *stmt = nullptr;

    int rc = sqlite3_prepare_v2(_db.handle(), sql, -1, &stmt, nullptr);
    checkPrepare(rc, _db.handle(), "ChannelRepository::getById/prepare");

    sqlite3_bind_int(stmt, 1, static_cast<int>(channelId));

    rc = sqlite3_step(stmt);
    checkStepRow(rc, _db.handle(), "ChannelRepository::getById/step");

    channels ch = rowToChannel(stmt);
    sqlite3_finalize(stmt);
    return ch;
}

std::vector<channels> ChannelRepository::listAll() const
{
    const char *sql =
        "SELECT ChannelID, Position, SalonName, CategoryID "
        "FROM Channels "
        "ORDER BY CategoryID ASC, Position ASC";

    sqlite3_stmt *stmt = nullptr;

    int rc = sqlite3_prepare_v2(_db.handle(), sql, -1, &stmt, nullptr);
    checkPrepare(rc, _db.handle(), "ChannelRepository::listAll/prepare");

    std::vector<channels> out;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        out.push_back(rowToChannel(stmt));
    }
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE)
        throw Error("ChannelRepository::listAll/step", std::string(sqlite3_errmsg(_db.handle())));

    return out;
}

std::vector<channels> ChannelRepository::listByCategory(unsigned int categoryId) const
{
    const char *sql =
        "SELECT ChannelID, Position, SalonName, CategoryID "
        "FROM Channels "
        "WHERE CategoryID = ? "
        "ORDER BY Position ASC";

    sqlite3_stmt *stmt = nullptr;

    int rc = sqlite3_prepare_v2(_db.handle(), sql, -1, &stmt, nullptr);
    checkPrepare(rc, _db.handle(), "ChannelRepository::listByCategory/prepare");

    sqlite3_bind_int(stmt, 1, static_cast<int>(categoryId));

    std::vector<channels> out;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        out.push_back(rowToChannel(stmt));
    }
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE)
        throw Error("ChannelRepository::listByCategory/step", std::string(sqlite3_errmsg(_db.handle())));

    return out;
}

void ChannelRepository::deleteById(unsigned int channelId)
{
    const char *sql = "DELETE FROM Channels WHERE ChannelID = ?";
    sqlite3_stmt *stmt = nullptr;

    int rc = sqlite3_prepare_v2(_db.handle(), sql, -1, &stmt, nullptr);
    checkPrepare(rc, _db.handle(), "ChannelRepository::deleteById/prepare");

    sqlite3_bind_int(stmt, 1, static_cast<int>(channelId));

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    checkStepDone(rc, _db.handle(), "ChannelRepository::deleteById/step");
}
