##
## EPITECH PROJECT, 2025
## B-NWP-400-STG-4-1-myftp-noe.carabin
## File description:
## Makefile
##

SRC	=	Src/main.c	\
		Src/CommandsFinder.c	\
		Src/InitServer.c	\
		Src/NetworkManagment.c	\
		Src/cmd/ping.c	\
		Src/cmd/join.c	\
		Src/cmd/send.c	\
		Src/cmd/channel_admin.c	\
		Src/cmd/msg.c	\

OBJ	=	$(SRC:.c=.o)

NAME =	my-messaging-server

CFLAGS = -g

all: $(NAME)

$(NAME):	$(OBJ)
	gcc -o $(NAME) $(OBJ) -lsqlite3

clean:
	rm -f $(OBJ)

fclean: clean
		rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re