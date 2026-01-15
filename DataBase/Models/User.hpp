#include "../Include.hpp"

struct user {
    int UserID;

    std::string UserName;
    Role_t role;
    
    bool IsConnected = false;
}user_t;