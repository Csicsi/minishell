#ifndef MINISHELL_H
# define MINISHELL_H

# include <unistd.h>
# include <stdio.h>
# include <stdlib.h>
# include <errno.h>
# include <sys/stat.h>
# include <linux/limits.h>
# include <fcntl.h>
# include <sys/wait.h>
# include <string.h>
# include <ctype.h>
# include <stdbool.h>
# include <signal.h>
# include <limits.h>
# include <readline/readline.h>
# include <readline/history.h>
# include "../libft/libft.h"

typedef enum e_in_out_flag
{
	IO_NONE_OR_SINGLE,
	IO_INPUT_FIRST,
	IO_OUTPUT_FIRST
}	t_in_out_flag;

typedef struct s_cmd
{
	char			**args;
	char			*input;
	char			*output;
	int				append_output;
	int				exit_status;
	struct s_cmd	*next;
	char			*heredoc_delim;
	bool			is_heredoc;
	bool			io_error;
	char			*heredoc_tempfile;
	t_in_out_flag	io_flag;
}	t_cmd;

typedef enum e_token_type
{
	TOKEN_WORD,
	TOKEN_OPERATOR,
	TOKEN_UNKNOWN,
	TOKEN_END
}	t_token_type;

typedef struct s_token
{
	t_token_type	type;
	char			*value;
	int				word;
	bool			is_expanded;
	struct s_token	*next;
}	t_token;

typedef struct s_data
{
	bool		exit_flag;
	int			last_exit_status;
	int			token_count;
	char		*input;
	char		**env_vars;
	int			word_index;
	bool		in_heredoc;
	bool		heredoc_single_quote;
	t_token		*tokens;
	t_cmd		*cmd_list;
}	t_data;

typedef struct s_parse_context
{
	int		arg_index;
	bool	has_input;
	bool	has_output;
}	t_parse_context;

typedef struct s_exec_data
{
	int		*prev_fd;
	int		pipe_fd[2];
	int		*num_children;
}	t_exec_data;

/* ********************************** */
/* functions pertaining to lexer.c    */
/* ********************************** */
int			lexer(t_data *data);
void		cleanup_data(t_data *data, bool free_env);
void		free_tokens(t_data *data);
int			count_tokens(t_token *tokens);
t_cmd		*initialize_cmd(void);
char		*expand_env_var(char *cursor, int last_exit_status, t_data *data);
char		*expand_env_var_in_str(char **ptr_to_cursor,
				int last_exit_status, t_data *data);
char		*extract_single_quoted_word(char *cursor, t_token *token);
char		*extract_double_quoted_word(char *cursor,
				t_token *token, t_data *data, int in_heredoc);
int			calculate_expanded_length(char *cursor,
				int last_exit_status, t_data *data);
void		handle_expanded_tokens(t_data *data);
void		join_tokens_in_same_word(t_data *data);
t_token		*create_token(int type, int word_index);
char		*check_operator(char *cursor, t_token *token);
char		*create_and_add_token(char *cursor,
				t_token **token_list, t_data *data);
char		*handle_operator_or_quote(char *cursor,
				t_token *new_token, t_data *data);
char		*extract_unquoted_word(char *cursor, t_token *new_token);
int			handle_env_variables(t_token *new_token, t_data *data);

/* ********************************** */
/* functions pertaining to execute.c  */
/* ********************************** */

/* execute.c */
int			execute_cmd_list(t_data *data);
int			validate_cmd_list(t_data *data);
int			validate_syntax(t_data *data);
int			check_for_brackets(t_data *data);
int			execute_single_cmd(t_cmd *cmd, t_data *data);
/* execute_utils1.c */
bool		initialize(t_data *data, char **env_vars, int argc, char **argv);
char		*get_input_line(t_data *data);
int			check_for_unclosed_quotes(t_data *data);
/* execute_utils2.c */
int			validate_token_syntax(t_token *tokens, t_data *data);
int			count_cmds(t_cmd *cmd_list);
int			count_words(t_token *tokens);
/* execute_utils3.c */
int			is_builtin(char *command_name);
int			execute_builtin(t_cmd *cmd, t_data *data, bool print_exit);
void		handle_heredoc(t_cmd *cmd_list, t_data *data);
char		*find_cmd_path(char **cmd_args, t_data *data);
/* execute_utils4.c */
int			update_last_command_env_var(t_data *data, char *cmd_path);
char		*get_directory_from_path(const char *path);
t_token		*find_token_by_value(t_token *tokens, const char *value);
/* execute_parse_tokens_utils1.c */
t_cmd		*parse_tokens(t_data *data);
bool		parse_single_token(t_data *data,
				t_cmd **current_cmd, t_parse_context *context);
/* execute_parse_tokens_utils2.c */
int			case_redirection(t_cmd *cur_cmd, int *arg_index, t_data *data);

/* ********************************** */
/* implementing the builtin functions */
/* ********************************** */
/* echo.c         */
int			builtin_echo(t_cmd *cmd);
/* cd.c           */
int			builtin_cd(t_cmd *cmd, t_data *data);
/* cd_utils1.c    */
char		*resolve_cdpath_if_needed(const char *path, t_data *data);
/* cd_utils2.c    */
int			ft_setenv(const char *varname, const char *value, t_data *data);
/* cd_utils3.c    */
//char		*ft_realpath(const char *path, char *resolved_path);
/* pwd.c          */
int			builtin_pwd(void);
/* export.c       */
int			builtin_export(t_cmd *cmd, t_data *data);
/* export_utils.c */
int			handle_export_wo_args(t_cmd *cmd, t_data *data);
int			is_valid_env_var_name(const char *name);
/* unset.c        */
int			builtin_unset(t_cmd *cmd, t_data *data);
/* env.c          */
int			builtin_env(t_data *data);
/* exit.c         */
int			builtin_exit(t_cmd *cmd, t_data *data, bool print_exit);

/* ********************************** */
/* cleanup.c 						  */
/* ********************************** */
void		free_string_array(char **string_array);

/* ********************************** */
/* pure utilities/ helper functions   */
/* ********************************** */
int			ft_fprintf(int fd, const char *format, ...);
char		*ft_strjoin_pipex(char *s1, char *s2);
char		*free_null(char *str);
char		*ft_getenv(char *look_for_match, char **envp);
char		*ft_strndup(const char *s, size_t n);
char		*ft_strcpy(char *dest, const char *src);
void		*ft_realloc(void *ptr, size_t old_size, size_t new_size);
char		*skip_spaces(char *cursor);
bool		is_all_spaces(char *str);

/* ********************************** */
/* signals.c 						  */
/* ********************************** */
void		handle_sigint(int sig);

#endif
