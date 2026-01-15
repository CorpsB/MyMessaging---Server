#include "../Include.hpp"

struct message_s {
    unsigned int MessageID;
    unsigned int ChannelID;
    unsigned int AuthorID;

    bool IsVisible;

    std::string Content;
    std::string CreatedAt;
    std::string EditedDate;
}message_t;
