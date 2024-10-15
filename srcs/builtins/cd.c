/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krabitsc <krabitsc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/15 12:09:22 by dcsicsak          #+#    #+#             */
/*   Updated: 2024/10/15 14:51:15 by krabitsc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/**
 * @brief Resolves special paths for the `cd` command, such as `-` (OLDPWD) and no argument (HOME).
 *
 * @param cmd The command structure containing arguments.
 * @param target_path Pointer to hold the resolved path if applicable.
 * @return int 0 if a special path is resolved, 1 if an error occurred, 0 if no special path was resolved.
 */
static int	resolve_special_paths(t_command *cmd, t_data *data, char **target_path)
{
	char	*curpath;

	(void)data;

	curpath = NULL;
	
	// Check if no arguments (or a ~) are passed, which means the user wants to go to HOME.
	if (cmd->args[1] == NULL || (cmd->args[1] && ft_strncmp(cmd->args[1], "~", 2) == 0))
	{
		curpath = ft_getenv(strdup("HOME"), data->env_vars);
		// If HOME is not set, display an error message.
		if (curpath == NULL || *curpath == '\0')
			return (ft_putstr_fd("minishell: cd: HOME not set\n",
					STDERR_FILENO), 1);
	}
	else if (cmd->args[1] && ft_strncmp(cmd->args[1], "-", 2) == 0)
	{
		curpath = ft_getenv(strdup("OLDPWD"), data->env_vars);
		// If OLDPWD is not set, display an error message.
		if (curpath == NULL)
			return (ft_putstr_fd("minishell: cd: OLDPWD not set\n",
					STDERR_FILENO), 1);
			//printf("%s\n", cwd);
		
	}
	// Check if the argument is `-`, which refers to the previous working directory (OLDPWD).
	else
		// No special path detected, returning 0 to indicate normal path resolution.
		return (0);

	// If no valid path is found, return an error.
	if (curpath == NULL)
		return (1);

	// Allocate memory and copy the resolved path.
	*target_path = strdup(curpath);
	return (0);
}

/**
 * @brief Resolves the target path for the `cd` command considering both special and normal paths.
 *
 * @param cmd The command structure containing arguments.
 * @param target_path Pointer to hold the resolved target path.
 * @return int 0 if the target path is successfully resolved, 1 if an error occurred.
 */
static int	resolve_target_path(t_command *cmd, t_data *data, char **target_path)
{
	char	*curpath;
	char	*resolved_path;
	
	// If there are more than two arguments, display an error message for too many arguments.
	if (cmd->args[1] != NULL && cmd->args[2] != NULL)
	{
		ft_putstr_fd("minishell: cd: too many arguments\n", STDERR_FILENO);
		return (1);
	}

	// First, check if any special paths need to be resolved (like `-` or HOME).
	if (resolve_special_paths(cmd, data, target_path) == 0 && *target_path != NULL)
		return (0);

	// Get the argument if it's a normal path.
	curpath = cmd->args[1];

	// If the path does not exist and it's a relative path, try to resolve using CDPATH.
	if (curpath && access(curpath, F_OK) != 0 && curpath[0] != '/')
	{
		resolved_path = resolve_cdpath_if_needed(curpath, data);
		if (resolved_path != NULL)
		{
			*target_path = resolved_path;
			return (0);
		}
	}

	// If no path is given (should not happen at this point), return an error.
	if (curpath == NULL)
		return (1);

	// Allocate memory and copy the given path.
	*target_path = strdup(curpath);
	return (0);
}

/**
 * @brief Changes the current directory to the specified path, updating environment variables.
 *
 * @param curpath The path to change the directory to.
 * @return int 0 if successful, 1 if an error occurred.
 */
static int	change_directory(const char *curpath, t_data *data)
{
	char	cwd[PATH_MAX];
	char	*normalized_path;

	// Normalize the provided path to handle `.` and `..` correctly.
	normalized_path = normalize_path(curpath);
	if (normalized_path == NULL)
		return (1);

	// Get the current working directory before changing it.
	if (getcwd(cwd, sizeof(cwd)) == NULL)
		return (perror("minishell: cd: getcwd"),free(normalized_path), 1);
	
	// Update OLDPWD with the current working directory.
	if (ft_setenv("OLDPWD", cwd, data) != 0)
		return (perror("minishell: cd: setenv OLDPWD"), free(normalized_path), 1);

	// Change the directory to the new path.
	if (chdir(normalized_path) != 0)
		return (perror("minishell: cd"), free(normalized_path), 1);

	// Get the new working directory after changing it.
	if (getcwd(cwd, sizeof(cwd)) == NULL)
		return (perror("minishell: cd: getcwd"), free(normalized_path), 1);
	
	// Update PWD with the current working directory.
	if (ft_setenv("PWD", cwd, data) != 0)
		return (perror("minishell: cd: setenv PWD"), free(normalized_path), 1);

	// Free the normalized path.
	free(normalized_path);

	// Update PWD environment variable with the new directory.
	return (update_directory_env(cwd, data));
}

/**
 * @brief The built-in `cd` command function, handling directory changes.
 *
 * @param cmd The command structure containing arguments.
 * @return int 0 if successful, 1 if an error occurred.
 */
int	builtin_cd(t_command *cmd, t_data *data)
{
	char	*target_path;
	int		result;

	target_path = NULL;

	// Resolve the target path based on the provided arguments.
	result = resolve_target_path(cmd, data, &target_path);

	// If the target path is resolved successfully, attempt to change the directory.
	if (result == 0 && target_path != NULL)
	{
		result = change_directory(target_path, data);
		free(target_path); // Free the allocated memory for the target path.
	}

	return (result);
}
