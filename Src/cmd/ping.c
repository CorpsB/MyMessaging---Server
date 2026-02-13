#include "../../Include/include.h"

void ping(char buf[1024], client_t **clients, int i, server_t *serv) {
    dprintf(clients[i]->fd.fd, "Pong\n");
}