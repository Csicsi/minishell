#include "../includes/minishell.h"

/**
 * @brief Handles the `export` builtin command.
 *
 * This function exports a variable, i.e. adds or updates a variable in the environment variables.
 *
 * @param varname_value The string in the format "varname=value".
 * @param envp The environment variables array.
 * @return int Returns 0 on success, or 1 if an error occurs.
 */
int	builtin_export(t_command *cmd, t_data *data)
{
	int		i;
	char	*equal_sign;
	char	*varname;
	//char	*value;

	char	*varname_value = cmd->args[1];
	char	**envp = data->env_vars;

	// Find the '=' sign in the argument
	equal_sign = ft_strchr(varname_value, '=');
	if (equal_sign == NULL)
	{
		// Handle case when only the variable name is provided without a value (just exporting the variable)
		printf("Error: invalid format, no '=' found in %s\n", varname_value);
		return (1);
	}

/**
 * @brief Helper function for 'handle_export_wo_args' to print env variables
 * @return void
 */
void	print_sorted_env_vars(char **env_vars)
{
	int		i;
	char	*equal_sign;

	// Search for the variable in the environment list
	i = 0;
	while (envp[i] != NULL)
	{
		// Compare the varname with the environment variable before '=' (or \0 for created by unassigned env var)
		if (ft_strncmp(envp[i], varname, ft_strlen(varname)) == 0 &&
    		(envp[i][ft_strlen(varname)] == '=' || envp[i][ft_strlen(varname)] == '\0'))
			break ;
		i++;
	}

	// If found, update its value
	if (envp[i] != NULL)
	{
		// Free the old value before assigning a new one
		free(envp[i]);
		envp[i] = strdup(varname_value);  // Duplicate the string into envp[i]
		if (!envp[i])
    		return (1);  // IMPLEMENT CLEAN UP Handle memory allocation failure
	}
	// If not found, add it to the environment list
	else
	{
		int	env_count = i; // i is at the end of the envp list

		// Allocate space for the new environment variable
		envp = realloc(envp, (env_count + 2) * sizeof(char *)); // +2 for the new variable and the null terminator
		if (!envp)
			return (1); // Handle memory allocation failure
		envp[env_count] = malloc(ft_strlen(varname_value) + 1);
		if (!envp[env_count])
			return (1); // Handle memory allocation failure
		sprintf(envp[env_count], "%s", varname_value);
		envp[env_count + 1] = NULL; // Null-terminate the array
	}

	// Free the temporary varname allocation
	free(varname);

	return (0);
}
