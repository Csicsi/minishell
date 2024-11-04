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
# include "utils.h"

typedef enum e_token_type
{
	TOKEN_WORD,
	TOKEN_OPERATOR,
	TOKEN_UNKNOWN,
	TOKEN_END
}	t_token_type;

typedef struct s_file
{
	char			*filename;
	bool			append;
	struct s_file	*next;
}	t_file;

typedef struct s_cmd
{
	char			**args;
	t_file			*input_files;
	t_file			*output_files;
	int				append_output;
	int				exit_status;
	struct s_cmd	*next;
	char			*heredoc_delim;
	bool			is_heredoc;
	bool			io_error;
	char			*heredoc_tempfile;
	bool			skip_execution;
}	t_cmd;

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

typedef struct s_exec_context
{
	int		prev_fd;
	int		pipe_fd[2];
	int		num_children;
	int		io_error_status;
	pid_t	*child_pids;
}	t_exec_context;

/* *******/
/* Lexer */
/* ***** */

// lexer.c
int			lexer(t_data *data);
// lexer_utils.c
t_token		*create_token(int type, int word_index);
char		*handle_operator_or_quote(char *cursor,
				t_token *new_token, t_data *data);
char		*extract_unquoted_word(char *cursor, t_token *new_token);
int			handle_env_variables(t_token *new_token, t_data *data);
// quotes.c
char		*extract_single_quoted_word(char *cursor, t_token *token);
char		*extract_double_quoted_word(char *cursor,
				t_token *token, t_data *data, int in_heredoc);
// expansion.c
char		*expand_env_var(char *cursor, int last_exit_status, t_data *data);
char		*expand_env_var_in_str(char **ptr_to_cursor,
				int last_exit_status, t_data *data);
// tokens_postprocess.c
void		handle_expanded_tokens(t_data *data);
// var_len.c
int			calculate_expanded_length(char *cursor,
				int last_exit_status, t_data *data);

/* ********* */
/* Execution */
/* ********* */

// execute_list.c
int			execute_cmd_list(t_data *data);
// execute_single.c
int			execute_single_cmd(t_cmd *cmd, t_data *data);
// execute_utils1.c
int			is_builtin(char *command_name);
int			execute_builtin(t_cmd *cmd, t_data *data, bool print_exit);
//char		*find_cmd_path(char **cmd_args, t_data *data, int *err_flag);
char		*find_cmd_path(char **cmd_args, t_data *data);
int			count_cmds(t_cmd *cmd_list);
// execute_utils2.c
int			update_last_command_env_var(t_data *data, char *cmd_path);
char		*get_directory_from_path(const char *path);
t_token		*find_token_by_value(t_token *tokens, const char *value);
// heredoc_utils.c
char		*generate_random_filename(void);
int			open_heredoc_file(t_cmd *cmd);
void		read_and_write_heredoc(t_cmd *cmd, t_data *data, int fd);
// heredoc.c
void		handle_heredoc(t_cmd *cmd_list, t_data *data);
// redirs.c
int			handle_input_redirection(t_cmd *cmd, t_data *data);
int			handle_output_redirection(t_cmd *cmd, t_data *data);
// validate_grammar.c
int			validate_cmd_list(t_data *data);
bool		check_input_error(t_file *input_file, t_cmd *cmd, t_data *data);
bool		check_output_error(t_file *output_file, t_cmd *cmd, t_data *data);
// validate_syntax.c
int			validate_syntax(t_data *data);

/* ******** */
/* Parsing */
/* ******** */

// parser.c
t_cmd		*parse_tokens(t_data *data);
t_cmd		*initialize_cmd(void);
int			count_tokens(t_token *tokens);
// token_handlers.c
bool		parse_single_token(t_data *data,
				t_cmd **current_cmd, t_parse_context *context);

/* ******** */
/* Builtins */
/* ******** */

// echo.c
int			builtin_echo(t_cmd *cmd);
// cd.c
int			builtin_cd(t_cmd *cmd, t_data *data);
// cd_utils1.c
char		*resolve_cdpath_if_needed(const char *path, t_data *data);
// cd_utils2.c
int			ft_setenv(const char *varname, const char *value, t_data *data);
// pwd.c
int			builtin_pwd(void);
// export.c
int			builtin_export(t_cmd *cmd, t_data *data);
// export_utils.c
int			handle_export_wo_args(t_cmd *cmd, t_data *data);
int			is_valid_env_var_name(const char *name);
// unset.c
int			builtin_unset(t_cmd *cmd, t_data *data);
// env.c
int			builtin_env(t_data *data);
// exit.c
int			builtin_exit(t_cmd *cmd, t_data *data, bool print_exit);

/* ******* */
/* Signals */
/* ******* */

// signals.c
void		handle_sigint(int sig);
void		setup_signal_handlers(void);

/* ************** */
/* Initialization */
/* ************** */

// initialize.c
char		**duplicate_env_vars(char **env_vars);
bool		initialize(t_data *data, char **env_vars, int argc, char **argv);

#endif
