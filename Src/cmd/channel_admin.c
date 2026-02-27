/*
** EPITECH PROJECT, 2025
** MyMessaging Server - Channel administration commands
**
** This file implements commands allowing clients to manage channels: creating,
** deleting and renaming channels. Each command operates on the SQLite
** database to keep the server's state consistent. When a channel is
** deleted, clients currently connected to that channel are returned to the
** default channel (ChannelID = 1). These commands do not enforce any
** permission checks; it is assumed that the client only exposes the actions
** to authorised users.
*/

#include <sqlite3.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#include "../../Include/include.h"

/*
 * Parse a CREATE command and insert a new channel into the Channels table.
 * Expected format: "CREATE <name>"
 */
int create_channel_cmd(char *cmd_line, client_t **client, int i, server_t *serv)
{
    if (!cmd_line || !serv || !serv->db)
        return -1;

    // Find the first space to separate the command from the name
    char *name = strchr(cmd_line, ' ');
    if (!name)
        return -1;
    name++; // skip the space
    // Skip any additional spaces
    while (*name == ' ') name++;
    if (*name == '\0')
        return -1;

    // Compute the next position: max(Position) + 1, default to 0
    int next_position = 0;
    sqlite3_stmt *stmt = NULL;
    const char *maxpos_sql = "SELECT COALESCE(MAX(Position) + 1, 0) FROM Channels;";
    if (sqlite3_prepare_v2(serv->db, maxpos_sql, -1, &stmt, NULL) != SQLITE_OK)
        return -1;
    int rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        next_position = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);

    // Insert the channel with CategoryID = 1 (default category)
    const char *insert_sql =
        "INSERT INTO Channels (Position, SalonName, CategoryID) VALUES (?, ?, 1);";
    if (sqlite3_prepare_v2(serv->db, insert_sql, -1, &stmt, NULL) != SQLITE_OK)
        return -1;
    sqlite3_bind_int(stmt, 1, next_position);
    sqlite3_bind_text(stmt, 2, name, -1, SQLITE_TRANSIENT);
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        return -1;
    }
    sqlite3_finalize(stmt);

    // Retrieve the ID of the newly created channel
    int new_id = (int)sqlite3_last_insert_rowid(serv->db);

    // Broadcast to all clients: notify addition of the new channel. The format is:
    // "ADDCHANNEL <id> <name>". Iterate through all connected clients and send the line
    // to those with a valid socket. We ignore errors when writing to sockets.
    if (client) {
        for (int k = 0; k < 200; ++k) {
            if (!client[k] || client[k]->fd.fd < 0)
                continue;
            dprintf(client[k]->fd.fd, "ADDCHANNEL %d %s\n", new_id, name);
        }
    }
    (void)i;
    return 0;
}

/*
 * Parse a DELETE command and remove a channel from the database.
 * Expected format: "DELETE <channelId>"
 */
int delete_channel_cmd(char *cmd_line, client_t **client, int i, server_t *serv)
{
    int channel_id;
    if (!cmd_line || !serv || !serv->db)
        return -1;
    if (sscanf(cmd_line, "DELETE %d", &channel_id) != 1)
        return -1;
    if (channel_id <= 0)
        return -1;

    sqlite3_stmt *stmt = NULL;
    // Delete messages for the channel
    const char *delete_messages_sql = "DELETE FROM Messages WHERE ChannelID = ?;";
    if (sqlite3_prepare_v2(serv->db, delete_messages_sql, -1, &stmt, NULL) != SQLITE_OK)
        return -1;
    sqlite3_bind_int(stmt, 1, channel_id);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    // Delete the channel itself
    const char *delete_channel_sql = "DELETE FROM Channels WHERE ChannelID = ?;";
    if (sqlite3_prepare_v2(serv->db, delete_channel_sql, -1, &stmt, NULL) != SQLITE_OK)
        return -1;
    sqlite3_bind_int(stmt, 1, channel_id);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    // Move any clients currently joined to this channel to default channel (1)
    if (client) {
        for (int k = 0; k < 200; ++k) {
            if (!client[k])
                continue;
            if (client[k]->current_channel == channel_id) {
                client[k]->current_channel = 1;
            }
        }
        // Broadcast deletion to all clients
        for (int k = 0; k < 200; ++k) {
            if (!client[k] || client[k]->fd.fd < 0)
                continue;
            dprintf(client[k]->fd.fd, "DELCHANNEL %d\n", channel_id);
        }
    }
    (void)i;
    return 0;
}

/*
 * Parse a RENAME command and update the channel name in the database.
 * Expected format: "RENAME <channelId> <newName>"
 */
int rename_channel_cmd(char *cmd_line, client_t **client, int i, server_t *serv)
{
    if (!cmd_line || !serv || !serv->db)
        return -1;

    // Skip the command word and capture the rest
    char *ptr = strchr(cmd_line, ' ');
    if (!ptr)
        return -1;
    ptr++;
    while (*ptr == ' ') ptr++;
    if (*ptr == '\0')
        return -1;
    // Parse channel ID
    char *end;
    long cid = strtol(ptr, &end, 10);
    if (end == ptr || cid <= 0)
        return -1;
    int channel_id = (int)cid;
    // Skip spaces after the channel ID
    while (*end == ' ') end++;
    if (*end == '\0')
        return -1;
    char *newName = end;

    sqlite3_stmt *stmt = NULL;
    const char *update_sql = "UPDATE Channels SET SalonName = ? WHERE ChannelID = ?;";
    if (sqlite3_prepare_v2(serv->db, update_sql, -1, &stmt, NULL) != SQLITE_OK)
        return -1;
    sqlite3_bind_text(stmt, 1, newName, -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, channel_id);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    // Broadcast rename to all clients. Format: "RENAMECHANNEL <id> <newName>"
    if (client) {
        for (int k = 0; k < 200; ++k) {
            if (!client[k] || client[k]->fd.fd < 0)
                continue;
            dprintf(client[k]->fd.fd, "RENAMECHANNEL %d %s\n", channel_id, newName);
        }
    }
    (void)i;
    return 0;
}

/*
 * Handle a request to list all channels. See header for details.
 */
int list_channels_cmd(char *cmd_line, client_t **client, int i, server_t *serv)
{
    (void)cmd_line;
    if (!client || !client[i] || !serv || !serv->db)
        return -1;
    // Send a clear directive to the requesting client
    dprintf(client[i]->fd.fd, "CLEARCHANNELS\n");

    sqlite3_stmt *stmt = NULL;
    const char *sql = "SELECT ChannelID, SalonName FROM Channels ORDER BY Position ASC, ChannelID ASC;";
    if (sqlite3_prepare_v2(serv->db, sql, -1, &stmt, NULL) != SQLITE_OK)
        return -1;
    while (1) {
        int rc = sqlite3_step(stmt);
        if (rc == SQLITE_ROW) {
            int channel_id = sqlite3_column_int(stmt, 0);
            const unsigned char *name = sqlite3_column_text(stmt, 1);
            if (!name)
                name = (const unsigned char *)"";
            dprintf(client[i]->fd.fd, "CHANNEL %d %s\n", channel_id, name);
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