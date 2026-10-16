NAME    = codexion
CC      = cc
CFLAGS  = -Wall -Wextra -Werror -pthread
SRC     = time.c parse.c say.c bits.c table.c give.c coder.c jobs.c main.c
OBJ     = $(SRC:.c=.o)
HEADERS = codexion.h

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJ)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re