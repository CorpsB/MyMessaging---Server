#include "../Include/include.h"

static void found_command(client_t **client, int i, server_t *serv)
{
    int size;

    size = read(client[i]->fd.fd, client[i]->cmd, 1023);
    client[i]->cmd[size] = '\0';
    if (strncmp(client[i]->cmd, "JOIN", 4) == 0)
        return join(client[i]->cmd, client, i, serv);
    if (strncmp(client[i]->cmd, "SEND", 4) == 0)
        return send_pro(client[i]->cmd, client, i, serv);

    if (strncmp(client[i]->cmd, "PING", 4) == 0)
        return ping(client[i]->cmd, client, i, serv);

    if (strncmp(client[i]->cmd, "CREATE", 6) == 0)
        return create_channel_cmd(client[i]->cmd, client, i, serv);
    if (strncmp(client[i]->cmd, "DELETE", 6) == 0)
        return delete_channel_cmd(client[i]->cmd, client, i, serv);
    if (strncmp(client[i]->cmd, "RENAME", 6) == 0)
        return rename_channel_cmd(client[i]->cmd, client, i, serv);

    if (strncmp(client[i]->cmd, "LIST", 4) == 0)
        return list_channels_cmd(client[i]->cmd, client, i, serv);

    if (strncmp(client[i]->cmd, "HISTORY", 7) == 0)
        return history_cmd(client[i]->cmd, client, i, serv);
}

void analyse_client(client_t **client, server_t *serv)
{
    for (int i = 1; i < 21; i++) {
        if (client[i]->fd.revents & POLLIN) {
            found_command(client, i, serv);
        }
    }
}