#pragma once

#include "../../Include.hpp"

class DataBase;

class MessageRepository {
    public:
        explicit MessageRepository(DataBase &db);

        void CreateMessage(std::string message, int author);
    private:
        DataBase &_db;
};