#include "RoleRepository.hpp"

RoleRepository::RoleRepository(DataBase &db) : _db(db) {}

int RoleRepository::roleId(Role_t role) const
{
    switch (role) {
        case USER: return 1;
        case MODERATOR: return 2;
        case ADMINISTRATOR: return 3;
        default: break;
    }
    throw Error("RoleRepository::roleId", "Unknown role enum value");
}

Role_t RoleRepository::roleFromId(int roleId) const
{
    switch (roleId) {
        case 1:
            return USER;
        case 2:
            return MODERATOR;
        case 3:
            return ADMINISTRATOR;
        default:
            break;
    }
    throw Error("RoleRepository::roleFromId", "Unknown role id");
}
