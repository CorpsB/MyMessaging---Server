/*
** EPITECH PROJECT, 2025
** B-NWP-400-STG-4-1-myftp-noe.carabin
** File description:
** include
*/

#ifndef INCLUDE_H
    #define INCLUDE_H

    #include <unistd.h>      // Système UNIX (read, write, close, fork)
    #include <sys/socket.h>  // Sockets (socket, bind, listen, accept, connect)
    #include <netinet/in.h>  // Structures de sockets (sockaddr_in)
    #include <arpa/inet.h>   // Conversion IP (inet_addr, inet_ntoa)
    #include <poll.h>        // Gestion d'événements avec poll
    #include <fcntl.h>       // Configuration des descripteurs de fichiers
    #include <sys/types.h>   // Types de base pour les appels système
    #include <sys/wait.h>    // Gestion des processus (wait, waitpid)
    #include <errno.h>       // Gestion des erreurs système
    #include <stdio.h>
    #include <stdlib.h>
    #include <sys/stat.h>
    #include <string.h>
    #include <stdbool.h>
    #include <sqlite3.h>


    #define BUFFER_SIZE 4096

//Structures :

typedef struct user_s {
    int id;
    char *username;
    char *pwd;
    bool islogin;
}user_t;

typedef struct server_s {
    int sock;
    unsigned short port;
    struct sockaddr_in *sockaddr;
    char home[4096];
    sqlite3 *db;
} server_t;

typedef struct client_s {
    struct pollfd fd;
    char cmd[1024];
    bool login;
    int current_channel;
    int user_id;
} client_t;

//Initialisation du serveur :
int init_strcut(char *port, char *path);
void process_client_events(client_t **client, server_t *serv, struct pollfd *poll_fds, int *numberClient);
int client_connexion(server_t *serv, char *path);
void analyse_client(client_t **client, server_t *serv);
void ping(char buf[1024], client_t **clients, int i, server_t *serv);
int join(char *cmd_line, client_t **client, int i, server_t *serv);
int send_pro(char buf[1024], client_t **clients, int i, server_t *serv);


#endif /* !PICTURE_HPP_ */