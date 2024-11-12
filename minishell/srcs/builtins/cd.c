/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcsicsak <dcsicsak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/20 10:22:02 by krabitsc          #+#    #+#             */
/*   Updated: 2024/11/12 13:24:01 by dcsicsak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

static int	resolve_special_paths(t_cmd *cmd,
		t_data *data, char **target_path)
{
	char	*curpath;

	curpath = NULL;
	if (cmd->args[1] == NULL || (cmd->args[1]
			&& ft_strncmp(cmd->args[1], "~", 2) == 0))
	{
		curpath = ft_getenv(ft_strdup("HOME"), data->env_vars);
		if (curpath == NULL || *curpath == '\0')
			return (ft_putstr_fd(": cd: HOME not set\n", STDERR_FILENO), 1);
	}
	else if (cmd->args[1] && ft_strncmp(cmd->args[1], "-", 2) == 0)
	{
		curpath = ft_getenv(ft_strdup("OLDPWD"), data->env_vars);
		if (curpath == NULL)
			return (ft_putstr_fd(": cd: OLDPWD not set\n",
					STDERR_FILENO), 1);
		ft_putstr_fd(curpath, STDOUT_FILENO);
		ft_putchar_fd('\n', STDOUT_FILENO);
	}
	else
		return (0);
	if (curpath == NULL)
		return (1);
	*target_path = ft_strdup(curpath);
	return (0);
}

static int	resolve_target_path(t_cmd *cmd,
		t_data *data, char **target_path)
{
	char	*curpath;
	char	*resolved_path;

	if (cmd->args[1] != NULL && cmd->args[2] != NULL)
	{
		ft_putstr_fd(": cd: too many arguments\n", STDERR_FILENO);
		return (1);
	}
	if (resolve_special_paths(cmd, data, target_path)
		== 0 && *target_path != NULL)
		return (0);
	curpath = cmd->args[1];
	if (curpath && access(curpath, F_OK) != 0 && curpath[0] != '/')
	{
		resolved_path = resolve_cdpath_if_needed(curpath, data);
		if (resolved_path != NULL)
		{
			*target_path = resolved_path;
			return (0);
		}
	}
	if (curpath == NULL)
		return (1);
	*target_path = ft_strdup(curpath);
	return (0);
}

static char	*normalize_path(const char *path)
{
	char	normalized_path[PATH_MAX];

	if (realpath(path, normalized_path) == NULL)
	{
		if (ft_strncmp(path, "-", 2) != 0)
			ft_fprintf(2, ": cd: %s: No such file or directory\n", path);
		return (NULL);
	}
	return (ft_strdup(normalized_path));
}

static int	update_env_paths(const char *normalized_path,
	const char *curpath, t_data *data)
{
	char	cwd[PATH_MAX];

	if (getcwd(cwd, sizeof(cwd)) == NULL)
		return (perror(": cd: getcwd"), 1);
	if (ft_setenv("OLDPWD", cwd, data) != 0)
		return (perror(": cd: setenv OLDPWD"), 1);
	if (chdir(normalized_path) != 0)
	{
		if (errno == ENOTDIR)
			ft_fprintf(STDERR_FILENO, "cd: %s: Not a directory\n", curpath);
		else if (errno == EACCES)
			ft_fprintf(STDERR_FILENO, "cd: %s: Permission denied\n", curpath);
		else
			ft_fprintf(STDERR_FILENO,
				"cd: %s: No such file or directory\n", curpath);
		return (1);
	}
	if (getcwd(cwd, sizeof(cwd)) == NULL)
		return (perror(": cd: getcwd"), 1);
	if (ft_setenv("PWD", cwd, data) != 0)
		return (perror(": cd: setenv PWD"), 1);
	return (0);
}

int	builtin_cd(t_cmd *cmd, t_data *data)
{
	char	*target_path;
	char	*normalized_path;
	int		result;

	target_path = NULL;
	result = resolve_target_path(cmd, data, &target_path);
	if (result == 0 && target_path != NULL)
	{
		normalized_path = normalize_path(target_path);
		if (normalized_path == NULL)
		{
			free(target_path);
			return (1);
		}
		result = update_env_paths(normalized_path, target_path, data);
		free(normalized_path);
		free(target_path);
	}
	return (result);
}
