#include "../Include/include.h"

int client_connexion(server_t *serv, char *path)
{
    client_t **client;
    struct pollfd poll_fds[200];
    int numberClient = 0;

    client = malloc(sizeof(client_t *) * 200);
    if (!client)
        return 1;
    init_client(client, serv, path);
    client[0]->fd.fd = serv->sock;
    client[0]->fd.events = POLLIN;
    client[0]->fd.revents = 0;
    while (true) {
        for (int i = 0; i <= numberClient; i++)
            poll_fds[i] = client[i]->fd;
        if (poll(poll_fds, numberClient + 1, -1) < 0)
            continue;
        process_client_events(client, serv, poll_fds, &numberClient);
    }
    return 0;
}

static void reset_client(client_t *client, server_t *serv)
{
    client->fd.fd = -1;
    client->fd.events = 0;
    client->fd.revents = 0;
    client->user_id = -1;
    client->login = false;
}

void remove_client(client_t *client, server_t *serv)
{
    if (!client)
        return;
    close(client->fd.fd);
    free(client);
    client = malloc(sizeof(client_t));
    if (!client)
        return;
    reset_client(client, serv);
}

void init_client(client_t **client, server_t *serv, char *path)
{
    realpath(path, serv->home);
    for (int i = 0; i < 200; i++) {
        client[i] = malloc(sizeof(client_t));
        client[i]->fd.fd = -1;
        client[i]->fd.events = 0;
        client[i]->fd.revents = 0;
        client[i]->login = false;
    }
}

static int new_client(client_t **client, server_t *serv, int nbr)
{
    socklen_t size = sizeof(struct sockaddr_in);
    int newfd = accept(serv->sock, (struct sockaddr *)serv->sockaddr, &size);

    for (int i = 0; i < 200; i++) {
        if (client[i]->fd.fd != -1)
            continue;
        client[i]->fd.fd = newfd;
        client[i]->fd.events = POLLIN;
        return (nbr + 1);
    }
}

static void completefd(int *table, struct pollfd *fd)
{
    for (int i = 0; i <= 200; i++) {
        fd[i].fd = -1;
        table[i + 1] = -1;
    }
}

void process_client_events(client_t **client, server_t *serv,
    struct pollfd *poll_fds, int *numberClient)
{
    for (int i = 0; i <= *numberClient; i++)
        client[i]->fd = poll_fds[i];
    if (client[0]->fd.revents & POLLIN) {
        *numberClient = new_client(client, serv, *numberClient);
        return;
    }
    for (int i = 1; i <= *numberClient; i++) {
        if (client[i]->fd.revents & (POLLHUP | POLLERR)) {
            close(client[i]->fd.fd);
            remove_client(client[i], serv);
            continue;
        }
    }
    analyse_client(client, serv);
}
