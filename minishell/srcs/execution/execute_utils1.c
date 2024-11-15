/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_utils1.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcsicsak <dcsicsak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/21 07:31:36 by krabitsc          #+#    #+#             */
/*   Updated: 2024/11/15 12:08:13 by dcsicsak         ###   ########.fr       */
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

int	execute_builtin(t_cmd *cmd, t_data *data,
	t_exec_context *ctx, bool print_exit)
{
	if (cmd->empty_redir)
	{
		cleanup_data(data, false);
		free(ctx->child_pids);
		return (1);
	}
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

static char	*ft_strreplace(const char *str, const char *old, const char *new)
{
	char	*result;
	char	*pos;
	size_t	old_len;
	size_t	new_len;
	size_t	result_len;

	pos = ft_strnstr(str, old, ft_strlen(str));
	if (!str || !old || !new || pos == NULL)
		return (ft_strdup(str));
	old_len = ft_strlen(old);
	new_len = ft_strlen(new);
	result_len = ft_strlen(str) - old_len + new_len;
	result = malloc(result_len + 1);
	if (!result)
		return (NULL);
	ft_strlcpy(result, str, pos - str + 1);
	ft_strlcat(result, new, result_len + 1);
	ft_strlcat(result, pos + old_len, result_len + 1);
	return (result);
}

static char	*find_in_path(char *cmd, t_data *data, t_exec_context *ctx)
{
	int		i;
	char	**allpath;
	char	*path_env;
	char	*path_for_execve;
	char	*path_curdir;

	path_curdir = ft_strjoin("./", cmd);
	path_env = ft_getenv(ft_strdup("PATH"), data->env_vars);
	if (!path_env || *path_env == '\0')
		return (ctx->path_exists = false, NULL);
	if (ft_strnstr(path_env, "::", ft_strlen(path_env)))
	{
		if (access(path_curdir, F_OK) == 0)
			ctx->path_exists = false;
		path_env = ft_strreplace(path_env, "::", ":.:");
	}
	if (path_env[0] == ':')
	{
		if (access(path_curdir, F_OK) == 0)
			ctx->path_exists = false;
		path_env = ft_strjoin(".:", path_env);
	}
	if (path_env[ft_strlen(path_env) - 1] == ':')
	{
		if (access(path_curdir, F_OK) == 0)
			ctx->path_exists = false;
		path_env = ft_strjoin(path_env, ":.");
	}
	allpath = ft_split(path_env, ':');
	i = -1;
	while (allpath[++i])
	{
		if (ft_strcmp(allpath[i], ".") == 0 || *allpath[i] == '\0')
			path_for_execve = ft_strdup(cmd);
		else
			path_for_execve = ft_strjoin_pipex(allpath[i], cmd);
		if (!path_for_execve)
			return (free_string_array(allpath), NULL);
		if (access(path_for_execve, F_OK | X_OK) == 0)
			return (free_string_array(allpath), path_for_execve);
		path_for_execve = free_null(path_for_execve);
	}
	return (free_string_array(allpath), NULL);
}

char	*find_cmd_path(char **cmd_args, t_data *data, t_exec_context *ctx)
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
	return (find_in_path(cmd_args[0], data, ctx));
}

int	count_cmds(t_cmd *cmd_list)
{
	int	count;

	count = 0;
	while (cmd_list)
	{
		count++;
		cmd_list = cmd_list->next;
	}
	return (count);
}
