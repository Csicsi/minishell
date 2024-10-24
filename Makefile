NAME = minishell
CC = cc
CFLAGS = -Wall -Wextra -Werror -g
LDFLAGS = -lreadline

LIBFT_DIR = ./libft
LIBFT = $(LIBFT_DIR)/libft.a

SRCS = srcs/lexer.c \
	   srcs/execute.c \
	   srcs/execute_utils1.c \
	   srcs/execute_utils2.c \
	   srcs/execute_utils3.c \
	   srcs/execute_utils4.c \
	   srcs/execute_parse_tokens_utils1.c \
	   srcs/cleanup.c \
	   srcs/builtins/echo.c \
	   srcs/builtins/cd.c \
	   srcs/builtins/cd_utils1.c \
	   srcs/builtins/cd_utils2.c \
	   srcs/builtins/pwd.c \
	   srcs/builtins/export.c \
	   srcs/builtins/export_utils.c \
	   srcs/builtins/unset.c \
	   srcs/builtins/env.c \
	   srcs/builtins/exit.c \
	   srcs/pure_utilities.c

OBJS = $(SRCS:.c=.o)

INCLUDES = -I ./includes -I $(LIBFT_DIR)

all: $(NAME)

$(NAME): $(LIBFT) $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBFT) -o $(NAME) $(LDFLAGS)

$(LIBFT):
	$(MAKE) -C $(LIBFT_DIR)

clean:
	$(MAKE) -C $(LIBFT_DIR) clean
	rm -f $(OBJS)

fclean: clean
	$(MAKE) -C $(LIBFT_DIR) fclean
	rm -f $(NAME)

re: fclean all

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

.PHONY: all clean fclean re
