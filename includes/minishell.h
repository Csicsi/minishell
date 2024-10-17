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
	TOKEN_END
}	t_token_type;

typedef struct s_token
{
	t_token_type	type;
	char			*value;
	int				word;
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

/* functions pertaining to lexer.c */
int		lexer(char *input, t_data *data, int last_exit_status);
void	cleanup_data(t_data *data, bool free_env);
void free_tokens(t_data *data);
t_command *parse_tokens(t_data *data);
int count_tokens(t_token *tokens);
//char	*expand_env_var(char *cursor, int last_exit_status);
//char	*expand_env_var_in_str(char **ptr_to_cursor, int last_exit_status);
char	*expand_env_var(char *cursor, int last_exit_status, t_data *data);
char	*expand_env_var_in_str(char **ptr_to_cursor, int last_exit_status, t_data *data);
/* functions pertaining to lexer.c */
/* implementing the builtin functions */
int		builtin_echo(t_command *cmd);
int		builtin_cd(t_command *cmd, t_data *data);
int		builtin_pwd(void);
int		builtin_export(t_command *cmd, t_data *data);
int		builtin_unset(t_command *cmd, t_data *data);
int		builtin_env(t_data *data);
int		builtin_exit(t_command *cmd, t_data *data, bool print_exit);
char	*resolve_cdpath_if_needed(const char *path, t_data *data);
int		update_directory_env(const char *cwd, t_data *data);
char	*normalize_path(const char *path);
/* pure utilities/ helper functions */
int		ft_fprintf(int fd, const char *format, ...);
char	*ft_strjoin_pipex(char *s1, char *s2);
void	free_array_of_strs(char **tab);
char	*free_null(char *str);
int		check_commands_in_tokens(t_token *tokens);
char	*ft_getenv(char *look_for_match, char **envp);
int		ft_setenv(const char *varname, const char *value, t_data *data);



#endif
