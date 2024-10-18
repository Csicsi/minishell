#include "../includes/minishell.h"

static int	resolve_special_paths(t_command *cmd,
		t_data *data, char **target_path)
{
	char	*curpath;

	(void)data;
	curpath = NULL;
	if (cmd->args[1] == NULL || (cmd->args[1]
			&& ft_strncmp(cmd->args[1], "~", 2) == 0))
	{
		curpath = ft_getenv(ft_strdup("HOME"), data->env_vars);
		if (curpath == NULL || *curpath == '\0')
			return (ft_putstr_fd(": cd: HOME not set\n",
					STDERR_FILENO), 1);
	}
	else if (cmd->args[1] && ft_strncmp(cmd->args[1], "-", 2) == 0)
	{
		curpath = ft_getenv(ft_strdup("OLDPWD"), data->env_vars);
		if (curpath == NULL)
			return (ft_putstr_fd(": cd: OLDPWD not set\n",
					STDERR_FILENO), 1);
	}
	else
		return (0);
	if (curpath == NULL)
		return (1);
	*target_path = ft_strdup(curpath);
	return (0);
}

static int	resolve_target_path(t_command *cmd,
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

static int	change_directory(const char *curpath, t_data *data)
{
	char	cwd[PATH_MAX];
	char	*normalized_path;

	normalized_path = normalize_path(curpath);
	if (normalized_path == NULL)
		return (1);
	if (getcwd(cwd, sizeof(cwd)) == NULL)
		return (perror(": cd: getcwd"), free(normalized_path), 1);
	if (ft_setenv("OLDPWD", cwd, data) != 0)
		return (perror(": cd: setenv OLDPWD"), free(normalized_path), 1);
	if (chdir(normalized_path) != 0)
		return (perror(": cd"), free(normalized_path), 1);
	if (getcwd(cwd, sizeof(cwd)) == NULL)
		return (perror(": cd: getcwd"), free(normalized_path), 1);
	if (ft_setenv("PWD", cwd, data) != 0)
		return (perror(": cd: setenv PWD"), free(normalized_path), 1);
	free(normalized_path);
	return (update_directory_env(cwd, data));
}

int	builtin_cd(t_command *cmd, t_data *data)
{
	char	*target_path;
	int		result;

	target_path = NULL;
	result = resolve_target_path(cmd, data, &target_path);
	if (result == 0 && target_path != NULL)
	{
		result = change_directory(target_path, data);
		free(target_path);
	}
	return (result);
}
