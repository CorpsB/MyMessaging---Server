#include <sqlite3.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#include "../../Include/include.h"

#ifndef MAX_CLIENTS
#define MAX_CLIENTS 200
#endif

int send_pro(char buf[1024], client_t **clients, int i, server_t *serv)
{
    int channel_id;
    char content[900];
    sqlite3_stmt *stmt = NULL;

    if (!buf || !clients || !clients[i] || !serv || !serv->db)
        return -1;

    if (sscanf(buf, "SEND %d %[^\n]", &channel_id, content) != 2)
        return -1;
    if (channel_id < 0)
        return -1;

    const char *insert_sql =
        "INSERT INTO Messages (Content, ChannelID, AuthorID, IsVisible) "
        "VALUES (?, ?, ?, 1);";

    if (sqlite3_prepare_v2(serv->db, insert_sql, -1, &stmt, NULL) != SQLITE_OK)
        return -1;
    sqlite3_bind_text(stmt, 1, content, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, channel_id);
    sqlite3_bind_int(stmt, 3, 1);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        return -1;
    }
    sqlite3_finalize(stmt);

    int message_id = (int)sqlite3_last_insert_rowid(serv->db);
    const char *select_sql =
        "SELECT strftime('%Y-%m-%dT%H:%M:%S', CreatedAt) "
        "FROM Messages WHERE MessageID = ?;";
    if (sqlite3_prepare_v2(serv->db, select_sql, -1, &stmt, NULL) != SQLITE_OK)
        return -1;
    sqlite3_bind_int(stmt, 1, message_id);

    // Copy the created_at timestamp into a local buffer. Pointers returned by
    // sqlite3_column_text remain valid only until the next call to
    // sqlite3_step() or sqlite3_finalize(), so we must copy the string before
    // finalising the prepared statement. Use a fixed-size buffer to avoid
    // referencing freed memory after sqlite3_finalize().
    char created_at_buf[32] = {0};
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        const unsigned char *tmp = sqlite3_column_text(stmt, 0);
        if (tmp) {
            // Copy at most sizeof(created_at_buf)-1 characters to ensure
            // null-termination.
            strncpy(created_at_buf, (const char *)tmp, sizeof(created_at_buf) - 1);
            created_at_buf[sizeof(created_at_buf) - 1] = '\0';
        }
    }
    sqlite3_finalize(stmt);

    // Broadcast the new message to all connected clients regardless of which
    // channel they have currently joined. The client will filter messages
    // client-side based on the channel ID. Originally this loop only sent
    // messages to clients whose current_channel matched channel_id. To
    // propagate messages to all clients, remove that condition.
    for (int k = 0; k < MAX_CLIENTS; ++k) {
        if (!clients[k] || clients[k]->fd.fd < 0)
            continue;
        dprintf(clients[k]->fd.fd,
                "MSG %d %d %d %s %d %s\n",
                message_id,
                channel_id,
                1,
                created_at_buf,
                1,
                content);
    }

    return 0;
}
