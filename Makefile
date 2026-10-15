NAME = codexion
CC = cc
CFLAGS = -Wall -Wextra -Werror -pthread
SRC = time.c parse.c say.c heap.c table.c give.c pick.c coder.c jobs.c main.c
OBJ = $(SRC:.c=.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJ)

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
