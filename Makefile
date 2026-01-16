##
## EPITECH PROJECT, 2025
## mydiscord lib
## File description:
## Makefile
##

.PHONY: all clean fclean re

SRC_DIR = .

SRC     = $(SRC_DIR)/Src/Main.cpp \
          $(SRC_DIR)/Net/Server.cpp \

FLAGS	= -std=c++20 -Wall -Wextra -Werror -g

OBJ	=	$(SRC:.cpp=.o)
NAME	=	MyMessaging-server

all: $(NAME)

$(NAME): $(OBJ)
	g++ -o $(NAME) $(OBJ) $(FLAGS)

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all
