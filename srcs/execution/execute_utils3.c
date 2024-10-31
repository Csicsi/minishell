/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_utils3.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcsicsak <dcsicsak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/21 07:31:36 by krabitsc          #+#    #+#             */
/*   Updated: 2024/10/31 14:55:22 by dcsicsak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

int	is_builtin(char *command_name)
{
	if (!command_name)
		return (0);
	if (ft_strncmp(command_name, "cd", 3) == 0)
		return (1);
	else if (ft_strncmp(command_name, "echo", 5) == 0)
		return (1);
	else if (ft_strncmp(command_name, "exit", 5) == 0)
		return (1);
	else if (ft_strncmp(command_name, "env", 4) == 0)
		return (1);
	else if (ft_strncmp(command_name, "export", 7) == 0)
		return (1);
	else if (ft_strncmp(command_name, "unset", 6) == 0)
		return (1);
	else if (ft_strncmp(command_name, "pwd", 4) == 0)
		return (1);
	return (0);
}

int	execute_builtin(t_cmd *cmd, t_data *data, bool print_exit)
{
	if (ft_strncmp(cmd->args[0], "cd", 3) == 0)
		return (builtin_cd(cmd, data));
	else if (ft_strncmp(cmd->args[0], "echo", 5) == 0)
		return (builtin_echo(cmd));
	else if (ft_strncmp(cmd->args[0], "exit", 5) == 0)
		return (builtin_exit(cmd, data, print_exit));
	else if (ft_strncmp(cmd->args[0], "env", 4) == 0)
		return (builtin_env(data));
	else if (ft_strncmp(cmd->args[0], "export", 7) == 0)
		return (builtin_export(cmd, data));
	else if (ft_strncmp(cmd->args[0], "unset", 6) == 0)
		return (builtin_unset(cmd, data));
	else if (ft_strncmp(cmd->args[0], "pwd", 4) == 0)
		return (builtin_pwd());
	return (1);
}

static char	*find_in_path(char *cmd, t_data *data)
{
	int		i;
	char	**allpath;
	char	*path_env;
	char	*path_for_execve;

	path_env = ft_getenv(ft_strdup("PATH"), data->env_vars);
	if (!path_env)
		return (NULL);
	allpath = ft_split(path_env, ':');
	i = -1;
	while (allpath[++i])
	{
		path_for_execve = ft_strjoin_pipex(allpath[i], cmd);
		if (!path_for_execve)
			return (free_string_array(allpath), NULL);
		if (access(path_for_execve, F_OK | X_OK) == 0)
		{
			free_string_array(allpath);
			return (path_for_execve);
		}
		path_for_execve = free_null(path_for_execve);
	}
	free_string_array(allpath);
	return (NULL);
}

char	*find_cmd_path(char **cmd_args, t_data *data)
{
	if (!cmd_args || !cmd_args[0])
		return (NULL);
	if (ft_strchr(cmd_args[0], '/') != NULL)
	{
		if (access(cmd_args[0], F_OK | X_OK) != 0)
			return (NULL);
		return (ft_strdup(cmd_args[0]));
	}
	if (access(cmd_args[0], F_OK | X_OK) == 0)
		return (ft_strdup(cmd_args[0]));
	return (find_in_path(cmd_args[0], data));
}
