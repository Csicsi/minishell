/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcsicsak <dcsicsak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/14 14:12:22 by dcsicsak          #+#    #+#             */
/*   Updated: 2024/10/04 11:27:29 by dcsicsak         ###   ########.fr       */
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
# include <string.h>
# include <ctype.h>
# include <stdbool.h>
# include <readline/readline.h>
# include <readline/history.h>
# include "../libft/libft.h"

typedef struct s_command
{
	char				*name;
	char				**args;
	char				*input;
	char				*output;
	int					append_output;
	int					exit_status;
	struct s_command	*next;
	char				**env_vars;
}	t_command;

typedef enum e_token_type
{
	TOKEN_WORD,
	TOKEN_OPERATOR,
	TOKEN_ENV_VAR,
	TOKEN_DOT,
	TOKEN_DOTDOT,
	TOKEN_UNKNOWN,
	TOKEN_END
}	t_token_type;

typedef struct s_token
{
	t_token_type	type;
	char			*value;
}	t_token;

typedef struct s_data
{
	bool		exit_flag;
	int			last_exit_status;
	int			token_count;
	t_token		*tokens;
	t_command	*cmd_list;
}	t_data;

int		builtin_cd(t_command *cmd);
char	*resolve_cdpath_if_needed(const char *path);
int		update_directory_env(const char *cwd, char ***env_vars);
char	*normalize_path(const char *path);
int		builtin_echo(t_command *cmd);
int		builtin_exit(t_command *cmd, bool *exit_flag);
//int		builtin_echo(t_command *cmd, int last_exit_status);
int		builtin_pwd(void);
void	free_tokens(t_data *data);
//int		lexer(char *input, t_token **tokens_ptr, int token_count);
int		lexer(char *input, t_token **tokens_ptr, int token_count, int last_exit_status);
int		count_tokens(char *cursor);

//char *expand_env_var(char **result, int *index, char *cursor);
char	*expand_env_var(char **result, int *index, char *cursor, int last_exit_status, int len_result);
char	*expand_env_var_in_str(char **ptr_to_cursor, int last_exit_status);


// prototypes in "pure_utilities.c":
char	*ft_strjoin_pipex(char *s1, char *s2);
void	free_array_of_strs(char **tab);
char	*free_null(char *str);


#endif
