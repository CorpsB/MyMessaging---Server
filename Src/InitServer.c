#include "../Include/include.h"

int init_strcut(char *port, char *path)
{
    server_t *serv = malloc(sizeof(server_t));
    socklen_t size = sizeof(struct sockaddr_in);

    serv->port = atoi(port);
    serv->sock = socket(AF_INET, SOCK_STREAM, 0);

    if (serv->sock == -1) {
        printf("[ERROR] - Socket error\n");
        exit(1);
    }

    serv->sockaddr = malloc(sizeof(struct sockaddr_in));
    serv->sockaddr->sin_family = AF_INET;
    serv->sockaddr->sin_port = htons(serv->port);
    serv->sockaddr->sin_addr.s_addr = INADDR_ANY;
    memset(serv->sockaddr->sin_zero, 0, sizeof(serv->sockaddr->sin_zero));

    if (bind(serv->sock, (struct sockaddr *) serv->sockaddr, size) == -1) {
        printf("[ERROR] - Socket bind\n");
        exit(1);
    }

    listen(serv->sock, 200);
    
    if (sqlite3_open("db.db", &serv->db) != SQLITE_OK) {
        printf("[ERROR] - Cannot open db.\n");
        exit(2);
    }
    return client_connexion(serv, path);
}