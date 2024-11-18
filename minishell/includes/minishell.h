/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcsicsak <dcsicsak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/12 13:27:30 by dcsicsak          #+#    #+#             */
/*   Updated: 2024/11/18 09:10:23 by dcsicsak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
# include <sys/ioctl.h>
# include <string.h>
# include <ctype.h>
# include <stdbool.h>
# include <signal.h>
# include <limits.h>
# include <readline/readline.h>
# include <readline/history.h>
# include "../libft/libft.h"
# include "utils.h"

extern volatile size_t	g_signal_value;

typedef enum e_token_type
{
	TOKEN_WORD,
	TOKEN_OPERATOR,
	TOKEN_UNKNOWN,
	TOKEN_EMPTY,
	TOKEN_ERROR,
	TOKEN_END
}	t_token_type;

typedef struct s_file
{
	char			*filename;
	bool			append;
	struct s_file	*next;
}	t_file;

typedef enum e_redirection_order
{
	NO_REDIRECTION,
	INPUT_FIRST,
	OUTPUT_FIRST
}	t_redirection_order;

typedef struct s_cmd
{
	char				**args;
	t_file				*input_files;
	t_file				*output_files;
	int					append_output;
	int					exit_status;
	struct s_cmd		*next;
	char				*heredoc_delim;
	bool				is_heredoc;
	bool				io_error;
	bool				empty_redir;
	char				*heredoc_tempfile;
	t_redirection_order	redirection_order;
	bool				skip_execution;
}	t_cmd;

typedef struct s_token
{
	t_token_type	type;
	char			*value;
	char			*old_value;
	int				word;
	bool			is_expanded;
	struct s_token	*next;
}	t_token;

typedef struct s_data
{
	int			pid;
	bool		exit_flag;
	int			last_exit_status;
	int			token_count;
	char		*input;
	char		**env_vars;
	int			word_index;
	bool		in_heredoc;
	bool		heredoc_single_quote;
	bool		syntax_error;
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
	bool	path_exists;
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
// execute_child.c
int			execute_command_in_list(t_cmd *current, t_data *data,
				t_exec_context *ctx);
// execute_single.c
int			execute_single_cmd(t_cmd *cmd, t_data *data,
				t_exec_context *ctx);
// execute_utils1.c
int			is_builtin(char *command_name);
int			execute_builtin(t_cmd *cmd, t_data *data,
				t_exec_context *ctx, bool print_exit);
//char		*find_cmd_path(char **cmd_args, t_data *data, int *err_flag);
char		*find_cmd_path(char **cmd_args, t_data *data, t_exec_context *ctx);
int			count_cmds(t_cmd *cmd_list);
// execute_utils2.c
int			update_last_command_env_var(t_data *data, char *cmd_path);
char		*get_directory_from_path(const char *path);
t_token		*find_token_by_value(t_token *tokens, const char *value);
// heredoc_utils.c
int			open_heredoc_file(t_cmd *cmd);
void		write_heredoc_line(int fd, char *line, t_data *data);
int			handle_signal_interrupt(t_cmd *cmd);
// heredoc.c
int			handle_heredoc(t_cmd *cmd_list, t_data *data);
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
//handle_io.c
bool		process_heredoc(t_cmd *cmd, t_token **tokens);
bool		handle_output(t_cmd *cmd, t_token **tokens, bool *has_output);
bool		handle_input(t_cmd *cmd, t_token **tokens, bool *has_input);

/* ******** */
/* Builtins */
/* ******** */

// echo.c
int			builtin_echo(t_cmd *cmd);
// cd.c
int			builtin_cd(t_cmd *cmd, t_data *data);
// cd_utils1.c
char		*resolve_cdpath_if_needed(const char *path, t_data *data);
char		*getcwd_from_env_var(char *cwd, size_t size, t_data *data);
// cd_utils2.c
int			ft_setenv(const char *varname, const char *value, t_data *data);
// cd_utils3.c
char		*ft_realpath(const char *path, char *resolved_path);
// pwd.c
int			builtin_pwd(t_data *data);
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
void		setup_signal_handlers(int context);

/* ************** */
/* Initialization */
/* ************** */

// initialize.c
char		**duplicate_env_vars(char **env_vars);
bool		initialize(t_data *data, char **env_vars, int argc, char **argv);

#endif
