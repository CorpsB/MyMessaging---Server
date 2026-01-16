#include "MessageRepository.hpp"

MessageRepository::MessageRepository(DataBase &db) : _db(db)
{
}

void MessageRepository::CreateMessage(std::string message, int author)
{
    std::string query = "INSERT INTO Messages (Content, ChannelID, AuthorID) VALUES (?, ?, ?)";
    sqlite3_stmt *stmt = nullptr;

    sqlite3_prepare_v2(_db.handle(), query.c_str(), -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, message.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, 1);
    sqlite3_bind_int(stmt, 3, author);

    const int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        throw Error("MessageRepository::createMessage", std::string(sqlite3_errmsg(_db.handle())));
    }
}
