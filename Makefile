NAME = cub3d
CC = gcc
RM = rm -rf
CFLAGS = -Werror -Wextra -Wall

SRCS = $(wildcard *.c)
OBJ = $(SRCS:.c=.o)
INCLUDE = -lmlx -framework OpenGL -framework AppKit

$(NAME): $(OBJ)
		make -C mlx
		$(CC)  $(INCLUDE) -o $(NAME) $(OBJ) mlx/libmlx.a

clean:
	$(RM) *.o

fclean: clean
	$(RM) $(NAME)
	make -C mlx clean

re: fclean all

.PHONY: all clean fclean re