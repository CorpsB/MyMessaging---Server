#include "../Include/include.h"
#include <signal.h>

int help_func(int exit)
{
    int nbr = open("./help.txt", O_RDONLY);
    struct stat information;
    char *message;

    stat("./help.txt", &information);
    message = malloc(sizeof(char) * (information.st_size + 1));
    read(nbr, message, information.st_size);
    write(1, message, information.st_size);
    close(nbr);
    free(message);
    return exit;
}

bool is_error(char *jingle)
{
    char path[4096];

    printf(jingle);
    if (atoi(jingle) > 65535)
        return true;
    return false;
}

int main(int ac, char **av)
{

    signal(SIGPIPE, SIG_IGN);
    if (av[1] && strcmp(av[1], "-h") == 0)
        return help_func(0);
    if (is_error(av[1]))
        return 84;
    init_strcut(av[1], av[2]);
}