#pragma once

#include "../../Include.hpp"

class DataBase;

class MessageRepository {
  public:
    explicit MessageRepository(DataBase &db);

    void CreateMessage(std::string message, int author);
    int createMessage(const std::string &content, unsigned int channelId, int authorId);

    std::vector<message_s> listByChannel(unsigned int channelId, bool includeHidden = false) const;

  private:
    message_s rowToMessage(sqlite3_stmt *stmt) const;

    DataBase &_db;
};