NAME = minishell
CC = cc
CFLAGS = -Wall -Wextra -Werror -g
LDFLAGS = -lreadline

LIBFT_DIR = ./libft
LIBFT = $(LIBFT_DIR)/libft.a

SRCS =	srcs/minishell_bonus.c \
		srcs/initialize_bonus.c \
		srcs/lexer/lexer_bonus.c \
		srcs/lexer/lexer_utils_bonus.c \
		srcs/lexer/expansion_bonus.c \
		srcs/lexer/var_len_bonus.c \
		srcs/lexer/quotes_bonus.c \
		srcs/lexer/tokens_postprocess_bonus.c \
		srcs/lexer/wildcards_bonus.c \
		srcs/lexer/validate_syntax_bonus.c \
		srcs/execution/execute_list_bonus.c \
		srcs/execution/execute_child_bonus.c \
		srcs/execution/execute_single_bonus.c \
		srcs/execution/redirs_bonus.c \
		srcs/execution/execute_utils1_bonus.c \
		srcs/execution/execute_utils2_bonus.c \
		srcs/execution/heredoc_bonus.c \
		srcs/execution/heredoc_utils_bonus.c \
		srcs/execution/find_cmd_path_bonus.c \
		srcs/parser/parser_bonus.c \
		srcs/parser/token_handlers_bonus.c \
		srcs/parser/handle_io_bonus.c \
		srcs/parser/validate_grammar_bonus.c \
		srcs/builtins/echo_bonus.c \
		srcs/builtins/cd_bonus.c \
		srcs/builtins/cd_utils1_bonus.c \
		srcs/builtins/cd_utils2_bonus.c \
		srcs/builtins/pwd_bonus.c \
		srcs/builtins/export_bonus.c \
		srcs/builtins/export_utils_bonus.c \
		srcs/builtins/unset_bonus.c \
		srcs/builtins/env_bonus.c \
		srcs/builtins/exit_bonus.c \
		srcs/builtins/ft_realpath_bonus.c \
		srcs/utils/utils1_bonus.c \
		srcs/utils/utils2_bonus.c \
		srcs/utils/utils3_bonus.c \
		srcs/utils/utils4_bonus.c \
		srcs/utils/utils5_bonus.c \
		srcs/utils/cleanup_bonus.c \
		srcs/utils/ft_fprintf_bonus.c \
		srcs/utils/ft_fprintf_utils_bonus.c \
		srcs/signals/signals_bonus.c

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
