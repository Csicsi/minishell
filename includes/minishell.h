/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcsicsak <dcsicsak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/14 14:12:22 by dcsicsak          #+#    #+#             */
/*   Updated: 2024/08/15 17:33:47 by dcsicsak         ###   ########.fr       */
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
# include "libft.h"

typedef struct s_command
{
	char				*name;
	char				**args;
	char				*input;
	char				*output;
	int					append_output;
	int					background;
	int					exit_status;
	struct s_command	*next;
	char				**env_vars;
}	t_command;

int		builtin_cd(t_command *cmd);
char	*resolve_cdpath_if_needed(const char *path);
int		update_directory_env(const char *cwd, char ***env_vars);
char	*normalize_path(const char *path);
int		builtin_echo(t_command *cmd);
int		ft_setenv(char ***env_vars, const char *name, const char *val, int ow);
void	free_split(char **split);

#endif
