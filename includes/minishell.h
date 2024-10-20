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
# include <readline/readline.h>
# include <readline/history.h>
# include "../libft/libft.h"

typedef struct s_command
{
	char				**args;
	char				*input;
	char				*output;
	int					append_output;
	int					exit_status;
	struct s_command	*next;
	char				*heredoc_delim;
	bool				is_heredoc;
}	t_command;

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
	t_token		*tokens;
	t_command	*cmd_list;
}	t_data;

/* ********************************** */
/* functions pertaining to lexer.c    */
/* ********************************** */
int			lexer(char *input, t_data *data, int last_exit_status);
void		cleanup_data(t_data *data, bool free_env);
void		free_tokens(t_data *data);
int			count_tokens(t_token *tokens);
char		*expand_env_var(char *cursor, int last_exit_status, t_data *data);
char		*expand_env_var_in_str(char **ptr_to_cursor, int last_exit_status, t_data *data);

/* ********************************** */
/* functions pertaining to execute.c  */
/* ********************************** */
/* execute_utils1.c */
bool		initialize(t_data *data, char **env_vars, int argc, char **argv);
char		*get_input_line(t_data *data);
/* execute_utils2.c */
int			check_commands_in_tokens(t_token *tokens, t_data *data);

t_command	*parse_tokens(t_data *data);

/* ********************************** */
/* implementing the builtin functions */
/* ********************************** */
/* echo.c         */
int			builtin_echo(t_command *cmd);
/* cd.c           */
int			builtin_cd(t_command *cmd, t_data *data);
/* cd_utils1.c    */
char		*resolve_cdpath_if_needed(const char *path, t_data *data);
/* cd_utils2.c    */
int			ft_setenv(const char *varname, const char *value, t_data *data);
/* cd_utils3.c    */
//char		*ft_realpath(const char *path, char *resolved_path);
/* pwd.c          */
int			builtin_pwd(void);
/* export.c       */
int			builtin_export(t_command *cmd, t_data *data);
/* export_utils.c */
int			handle_export_wo_args(t_command *cmd, t_data *data);
int			is_valid_env_var_name(const char *name);
/* unset.c        */
int			builtin_unset(t_command *cmd, t_data *data);
/* env.c          */
int			builtin_env(t_data *data);
/* exit.c         */
int			builtin_exit(t_command *cmd, t_data *data, bool print_exit);

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

#endif
