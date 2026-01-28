#pragma once

#include "../../Include.hpp"

class DataBase;

class ChannelRepository {
  public:
    explicit ChannelRepository(DataBase &db);

    int createChannel(const std::string &name, unsigned int position, unsigned int categoryId);
    channels getById(unsigned int channelId) const;

    std::vector<channels> listAll() const;
    std::vector<channels> listByCategory(unsigned int categoryId) const;

    void deleteById(unsigned int channelId);

  private:
    channels rowToChannel(sqlite3_stmt *stmt) const;
    DataBase &_db;
};
