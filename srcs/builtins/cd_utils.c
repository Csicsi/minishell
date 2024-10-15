/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd_utils.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krabitsc <krabitsc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/15 15:43:02 by dcsicsak          #+#    #+#             */
/*   Updated: 2024/10/15 11:39:55 by krabitsc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"


/**
 * @brief Checks if an environment variable exists and updates or creates it.
 *
 * @param varname The name of the environment variable.
 * @param value The value to set for the environment variable.
 * @param data Pointer to the data structure containing environment variables.
 * @return int Returns 0 on success, or 1 if an error occurs.
 */

int	ft_setenv(const char *varname, const char *value, t_data *data)
{
	int		i;
	char	*varname_value;

	int len_varname = ft_strlen(varname);
	int len_value = ft_strlen(value);
	// Create the full "varname=value" string
	varname_value = malloc(len_varname + len_value + 2); // +2 for '=' and '\0'
	if (!varname_value)
		return (1);  // Handle memory allocation failure
	// copy the varname and value into the result
	ft_strlcpy(varname_value, varname, len_varname + 1);  // Copy the variable name
	varname_value[len_varname] = '=';  // Add the '=' sign
	ft_strlcpy(varname_value + len_varname + 1, value, len_value + 1);  // Copy the value
	
	// Search for the variable in the environment list
	i = 0;
	while (data->env_vars[i] != NULL)
	{
		// Compare the varname with the environment variable before '='
		// (or for declared but unassigned env_var (e.g. with "export TEST"):
		// compare the varname_value with the environment variable before \0)
		if (ft_strncmp(data->env_vars[i], varname, ft_strlen(varname)) == 0 &&
			(data->env_vars[i][ft_strlen(varname)] == '=' || data->env_vars[i][ft_strlen(varname)] == '\0'))
			break ;
		i++;
	}
		// If found, update its value
		if (data->env_vars[i] != NULL)
		{
			if (ft_strchr(varname_value, '='))
			{
				// Free the old value before assigning a new one
				free(data->env_vars[i]);
				data->env_vars[i] = strdup(varname_value);  // Duplicate the string into data->env_vars[i]
				if (!data->env_vars[i])
    				return (1);  // IMPLEMENT CLEAN UP Handle memory allocation failure
				// Free the temporary varname_value allocation
				free(varname_value);
			}
		}
		// If not found, add it to the environment list
		else
		{
			int	env_count = i; // i is at the end of the data->env_vars list
			// Allocate space for the new environment variable
			data->env_vars = realloc(data->env_vars, (env_count + 2) * sizeof(char *)); // +1 for the new variable (since the count for NULL remains included))
			if (!data->env_vars)
				return (1); // Handle memory allocation failure
			data->env_vars[env_count] = strdup(varname_value);  // Duplicate the string into data->env_vars[env_count]
			if (!data->env_vars[env_count])
    			return (1);  // IMPLEMENT CLEAN UP Handle memory allocation failure
			data->env_vars[env_count + 1] = NULL; // Null-terminate the array
			// Free the temporary varname_value allocation
			free(varname_value);
		}
	return (0);
}


/**
 * @brief Concatenates a directory path and an argument to form a complete path.
 *
 * @param result The buffer to store the concatenated result.
 * @param dir The directory part of the path.
 * @param arg The argument to be appended to the directory.
 */
static void	concatenate_path(char *result, const char *dir, const char *arg)
{
    // Copy the directory path to the result buffer.
	while (*dir)
		*result++ = *dir++;

    // Add a '/' if the directory path does not end with one.
	if (*(result - 1) != '/')
		*result++ = '/';

    // Copy the argument to the result buffer.
	while (*arg)
		*result++ = *arg++;

    // Null-terminate the resulting path.
	*result = '\0';
}

/**
 * @brief Resolves a path using the CDPATH environment variable.
 *
 * @param cdpath The CDPATH string containing multiple paths separated by ':'.
 * @param arg The argument (target directory) provided to `cd`.
 * @return char* The resolved path if it exists, or NULL if not found.
 */
static char	*resolve_cdpath(const char *cdpath, const char *arg)
{
	char	temp_path[PATH_MAX];
	char	*token;
	int		token_len;

	token = (char *)cdpath;
	while (*token)
	{
        // Determine the length of the current path in CDPATH (delimited by ':').
		token_len = 0;
		while (token[token_len] && token[token_len] != ':')
			token_len++;

        // If the token is within the buffer size, process it.
		if (token_len < PATH_MAX)
		{
            // Copy the token (CDPATH directory) to the temporary buffer.
			ft_strlcpy(temp_path, token, token_len + 1);

            // Concatenate the target directory argument.
			concatenate_path(temp_path, temp_path, arg);

            // Check if the resulting path exists.
			if (access(temp_path, F_OK) == 0)
				return (ft_strdup(temp_path)); // Return the valid path.
		}

        // Move to the next path token in CDPATH.
		token += token_len;
		if (*token == ':')
			token++;
	}
	return (NULL); // Return NULL if no valid path is found.
}

/**
 * @brief Normalizes a given path by resolving symbolic links, `.` and `..`.
 *
 * @param path The path to be normalized.
 * @return char* The normalized absolute path, or NULL if normalization fails.
 */
char	*normalize_path(const char *path)
{
	char	normalized_path[PATH_MAX];

    // Use realpath to resolve and normalize the path.
	if (realpath(path, normalized_path) == NULL)
	{
		//perror("minishell: cd: realpath");
		fprintf(stderr, "minishell: cd: %s: No such file or directory\n", path);
		return (NULL);
	}
	return (ft_strdup(normalized_path)); // Return the normalized path.
}

/**
 * @brief Resolves the path using CDPATH if needed.
 *
 * @param path The target directory path provided to `cd`.
 * @return char* The resolved path using CDPATH, or NULL if no resolution is needed.
 */
char	*resolve_cdpath_if_needed(const char *path, t_data *data)
{
	char	*cdpath;

	cdpath = ft_getenv(strdup("CDPATH"), data->env_vars);
	if (cdpath != NULL)
		return (resolve_cdpath(cdpath, path)); // Attempt to resolve using CDPATH.
	return (NULL); // Return NULL if CDPATH is not set or no resolution is possible.
}

/**
 * @brief Updates the environment variables after changing the directory.
 *
 * @param cwd The new current working directory.
 * @return int 0 if successful, 1 if an error occurred.
 */
int	update_directory_env(const char *cwd, t_data *data)
{
	(void)data;
	
    // Update the PWD environment variable with the new current working directory.
	if (setenv("PWD", cwd, 1) != 0)
	{
		perror("minishell: cd: setenv PWD");
		return (1);
	}

    // Print the new directory (if required by the shell).
	//printf("%s\n", cwd);
	return (0);
}
