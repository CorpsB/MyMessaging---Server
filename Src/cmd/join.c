#include <sqlite3.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#include "../../Include/include.h"

int join(char *cmd_line, client_t **client, int i, server_t *serv)
{
    int channel_id;
    sqlite3_stmt *stmt = NULL;

    if (!cmd_line || !client || !client[i] || !serv || !serv->db)
        return -1;

    if (sscanf(cmd_line, "JOIN %d", &channel_id) != 1)
        return -1;
    if (channel_id < 0)
        return -1;

    client[i]->current_channel = channel_id;

    const char *sql =
        "SELECT MessageID, ChannelID, AuthorID, "
        "strftime('%Y-%m-%dT%H:%M:%S', CreatedAt) AS CreatedAt, "
        "IsVisible, Content "
        "FROM Messages "
        "WHERE ChannelID = ? AND IsVisible IN (0,1) "
        "ORDER BY MessageID DESC "
        "LIMIT 50;";

    if (sqlite3_prepare_v2(serv->db, sql, -1, &stmt, NULL) != SQLITE_OK)
        return -1;
    sqlite3_bind_int(stmt, 1, channel_id);

    while (1) {
        int rc = sqlite3_step(stmt);
        if (rc == SQLITE_ROW) {
            int msg_id = sqlite3_column_int(stmt, 0);
            int ch_id = sqlite3_column_int(stmt, 1);
            int author_id = sqlite3_column_int(stmt, 2);
            const unsigned char *created_at = sqlite3_column_text(stmt, 3);
            int visible = sqlite3_column_int(stmt, 4);
            const unsigned char *content = sqlite3_column_text(stmt, 5);

            dprintf(client[i]->fd.fd,
                    "MSG %d %d %d %s %d %s\n",
                    msg_id,
                    ch_id,
                    author_id,
                    created_at ? (const char *)created_at : "",
                    visible ? 1 : 0,
                    content ? (const char *)content : "");
        } else if (rc == SQLITE_DONE) {
            break;
        } else {
            sqlite3_finalize(stmt);
            return -1;
        }
    }

    sqlite3_finalize(stmt);
    return 0;
}
