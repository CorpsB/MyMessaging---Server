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

    const char *created_at = "";
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        const unsigned char *tmp = sqlite3_column_text(stmt, 0);
        if (tmp)
            created_at = (const char *)tmp;
    }
    sqlite3_finalize(stmt);

    for (int k = 0; k < MAX_CLIENTS; ++k) {
        if (!clients[k] || clients[k]->fd.fd < 0)
            continue;

        if (clients[k]->current_channel != channel_id)
            continue;

        dprintf(clients[k]->fd.fd,
                "MSG %d %d %d %s %d %s\n",
                message_id,
                channel_id,
                1,
                created_at,
                1,
                content);
    }

    return 0;
}
