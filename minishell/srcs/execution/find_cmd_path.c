/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   find_in_path.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcsicsak <dcsicsak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/15 17:06:51 by dcsicsak          #+#    #+#             */
/*   Updated: 2024/11/15 17:07:18 by dcsicsak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

static char	*normalize_path_env(char *path_env, char *path_curdir,
	t_exec_context *ctx)
{
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
	return (path_env);
}

static char	**parse_path_env(char *cmd, t_data *data,
	char **path_curdir, t_exec_context *ctx)
{
	char	*path_env;
	char	**allpath;

	*path_curdir = ft_strjoin("./", cmd);
	path_env = ft_getenv(ft_strdup("PATH"), data->env_vars);
	if (!path_env || *path_env == '\0')
	{
		ctx->path_exists = false;
		return (NULL);
	}
	path_env = normalize_path_env(path_env, *path_curdir, ctx);
	if (!path_env)
		return (NULL);
	allpath = ft_split(path_env, ':');
	return (allpath);
}

static char	*find_executable_in_paths(char **allpath, char *cmd)
{
	int		i;
	char	*path_for_execve;

	i = -1;
	while (allpath[++i])
	{
		if (ft_strcmp(allpath[i], ".") == 0 || *allpath[i] == '\0')
			path_for_execve = ft_strdup(cmd);
		else
			path_for_execve = ft_strjoin_pipex(allpath[i], cmd);
		if (!path_for_execve)
			return (NULL);
		if (access(path_for_execve, F_OK | X_OK) == 0)
			return (path_for_execve);
		path_for_execve = free_null(path_for_execve);
	}
	return (NULL);
}

static char	*find_in_path(char *cmd, t_data *data,
	t_exec_context *ctx)
{
	char	**allpath;
	char	*path_for_execve;
	char	*path_curdir;

	allpath = parse_path_env(cmd, data, &path_curdir, ctx);
	if (!allpath)
		return (NULL);
	path_for_execve = find_executable_in_paths(allpath, cmd);
	free_string_array(allpath);
	return (path_for_execve);
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
	return (find_in_path(cmd_args[0], data, ctx));
}
