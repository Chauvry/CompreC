NAME = CompreC

SRCS =  menu/menu.c                           \
        main.c                                \
        zip/gestionZip.c                      \

OBJS = $(SRCS:.c=.o)

CC = gcc

RM = rm -f

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) -o $(NAME) $(OBJS) -lncurses -lzip

$(OBJS): %.o: %.c
	$(CC) -c $< -o $@

clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
