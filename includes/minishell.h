/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcsicsak <dcsicsak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/14 14:12:22 by dcsicsak          #+#    #+#             */
/*   Updated: 2024/09/26 09:52:14 by dcsicsak         ###   ########.fr       */
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
	TOKEN_WILDCARD,
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

int		builtin_cd(t_command *cmd);
char	*resolve_cdpath_if_needed(const char *path);
int		update_directory_env(const char *cwd, char ***env_vars);
char	*normalize_path(const char *path);
int		builtin_echo(t_command *cmd);
int		builtin_pwd(t_command *cmd);
void	free_tokens(t_token *tokens, int token_count);
int		lexer(char *input, t_token **tokens_ptr, int token_count);
int		count_tokens(char *cursor);


#endif
