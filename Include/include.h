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

/* Channel administration commands */
int create_channel_cmd(char *cmd_line, client_t **client, int i, server_t *serv);
int delete_channel_cmd(char *cmd_line, client_t **client, int i, server_t *serv);
int rename_channel_cmd(char *cmd_line, client_t **client, int i, server_t *serv);

/*
 * Handle a request to list all channels. When a client sends the command "LIST",
 * the server will respond by first sending a CLEARCHANNELS line to instruct the
 * client to clear its local channel list, followed by one CHANNEL line per
 * channel in the database (format: "CHANNEL <id> <name>").
 *
 * @param cmd_line The raw command line received from the client (ignored).
 * @param client An array of pointers to connected clients used for broadcasting.
 * @param i The index of the requesting client in the array.
 * @param serv The server state containing the SQLite database.
 * @return 0 on success, -1 on error.
 */
int list_channels_cmd(char *cmd_line, client_t **client, int i, server_t *serv);

/*
 * Send the complete set of messages stored in the database to the requesting
 * client. Implemented in Src/cmd/msg.c. The server does not broadcast
 * these messages to other clients; it merely retrieves the stored history.
 */
int history_cmd(char *cmd_line, client_t **client, int i, server_t *serv);


#endif /* !PICTURE_HPP_ */