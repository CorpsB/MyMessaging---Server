#include "MessageRepository.hpp"

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

MessageRepository::MessageRepository(DataBase &db) : _db(db) {}

void MessageRepository::CreateMessage(std::string message, int author)
{
    (void)createMessage(message, 1, author);
}

int MessageRepository::createMessage(const std::string &content, unsigned int channelId, int authorId)
{
    const char *sql = "INSERT INTO Messages (Content, ChannelID, AuthorID) VALUES (?, ?, ?)";
    sqlite3_stmt *stmt = nullptr;

    int rc = sqlite3_prepare_v2(_db.handle(), sql, -1, &stmt, nullptr);
    checkPrepare(rc, _db.handle(), "MessageRepository::createMessage/prepare");

    sqlite3_bind_text(stmt, 1, content.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, static_cast<int>(channelId));
    sqlite3_bind_int(stmt, 3, authorId);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    checkStepDone(rc, _db.handle(), "MessageRepository::createMessage/step");

    return static_cast<int>(sqlite3_last_insert_rowid(_db.handle()));
}

message_s MessageRepository::rowToMessage(sqlite3_stmt *stmt) const
{
    message_s m;

    m.MessageID = static_cast<unsigned int>(sqlite3_column_int(stmt, 0));

    const unsigned char *content = sqlite3_column_text(stmt, 1);
    if (content != nullptr)
        m.Content = reinterpret_cast<const char *>(content);
    else
        m.Content = "";

    m.ChannelID = static_cast<unsigned int>(sqlite3_column_int(stmt, 2));

    const unsigned char *created = sqlite3_column_text(stmt, 3);
    if (created != nullptr)
        m.CreatedAt = reinterpret_cast<const char *>(created);
    else
        m.CreatedAt = "";

    m.IsVisible = sqlite3_column_int(stmt, 4) != 0;

    m.AuthorID = static_cast<unsigned int>(sqlite3_column_int(stmt, 5));

    const unsigned char *edited = sqlite3_column_text(stmt, 6);
    if (edited != nullptr)
        m.EditedDate = reinterpret_cast<const char *>(edited);
    else
        m.EditedDate = "";

    return m;
}

std::vector<message_s> MessageRepository::listByChannel(unsigned int channelId, bool includeHidden) const
{
    const char *sqlVisible =
        "SELECT MessageID, Content, ChannelID, CreatedAt, IsVisible, AuthorID, EditedDate "
        "FROM Messages "
        "WHERE ChannelID = ? AND IsVisible = 1 "
        "ORDER BY MessageID ASC";

    const char *sqlAll =
        "SELECT MessageID, Content, ChannelID, CreatedAt, IsVisible, AuthorID, EditedDate "
        "FROM Messages "
        "WHERE ChannelID = ? "
        "ORDER BY MessageID ASC";

    const char *sql = includeHidden ? sqlAll : sqlVisible;

    sqlite3_stmt *stmt = nullptr;
    int rc = sqlite3_prepare_v2(_db.handle(), sql, -1, &stmt, nullptr);
    checkPrepare(rc, _db.handle(), "MessageRepository::listByChannel/prepare");

    sqlite3_bind_int(stmt, 1, static_cast<int>(channelId));

    std::vector<message_s> out;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        out.push_back(rowToMessage(stmt));
    }
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE)
        throw Error("MessageRepository::listByChannel/step", std::string(sqlite3_errmsg(_db.handle())));

    return out;
}