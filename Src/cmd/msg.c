/*
** EPITECH PROJECT, 2025
** MyMessaging Server - Message-related commands
**
** This file implements functions related to message retrieval beyond the
** default per-channel history provided by the JOIN command. In particular,
** the history_cmd function allows a client to retrieve the complete set of
** messages stored in the database across all channels. Each message is
** emitted using the same "MSG" protocol format as live messages.
*/

#include <sqlite3.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#include "../../Include/include.h"

/*
 * Send all messages stored in the database to a single client. The format
 * matches the live message broadcast: "MSG <id> <channelId> <authorId> <createdAt> <visible> <content>".
 * Messages are sent in ascending order of their ID so that the client
 * receives them chronologically. Invisible messages (IsVisible = 0) are
 * included as well so that the client can filter them on its side if
 * necessary.
 */
int history_cmd(char *cmd_line, client_t **client, int i, server_t *serv)
{
    (void)cmd_line;
    if (!client || !client[i] || !serv || !serv->db)
        return -1;

    sqlite3_stmt *stmt = NULL;
    const char *sql =
        "SELECT MessageID, ChannelID, AuthorID, "
        "strftime('%Y-%m-%dT%H:%M:%S', CreatedAt) AS CreatedAt, "
        "IsVisible, Content "
        "FROM Messages "
        "ORDER BY MessageID ASC;";

    if (sqlite3_prepare_v2(serv->db, sql, -1, &stmt, NULL) != SQLITE_OK)
        return -1;
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