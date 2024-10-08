#include "../includes/minishell.h"

/**
 * @brief Checks validity of environment variable name
 *
 * Checks if the environment variable has a valid name according to POSIX standard
 *
 * @param cmd
 * @param data->env_vars The environment variables array.
 * @return int Returns 0 on success, or 1 if an error occurs.
 */
static int	is_valid_env_var_name(const char *name)
{
    if (!ft_isalpha(*name) && *name != '_')  // Must start with letter or underscore
        return (0);
    while (*name) {
        if (!ft_isalnum(*name) && *name != '_')  // Must be alphanumeric or underscore
            return (0);
        name++;
    }
    return (1);
}


/**
 * @brief Helper function that sorts env variables alphabetically
 * This function sorts the char** of environment variables alphabetically. 
 * Adopted from Piscine C06 ft_sort_params.c
 * @return void
 */
void	sort_env_vars(char **env_vars, int count)
{
	int		i;
	int		j;
	char	*temp;

	i = 0;
	while (i < count - 1)
	{
		j = i + 1;
		while (j < count)
		{
			if (ft_strcmp(env_vars[i], env_vars[j]) > 0)
			{
				// Swap the two strings
				temp = env_vars[i];
				env_vars[i] = env_vars[j];
				env_vars[j] = temp;
			}
			j++;
		}
		i++;
	}
}


/**
 * @brief Helper function for 'handle_export_wo_args' to print env variables
 * @return void
 */
void	print_sorted_env_vars(char **env_vars)
{
	int		i;
	char	*equal_sign;

	i = 0;
	while (env_vars[i] != NULL)
	{
		// Check if the variable contains an '=' sign (assigned or unassigned)
		equal_sign = ft_strchr(env_vars[i], '=');
		if (equal_sign)
		{
			// Print assigned variables as: declare -x VAR="value"
			printf("declare -x ");
			printf("%.*s", (int)(equal_sign - env_vars[i]), env_vars[i]); // Print variable name up to '='
			printf("=\"%s\"\n", equal_sign + 1); // Print value in quotes after '='
		}
		else
		{
			// Print unassigned variables as: declare -x VAR
			printf("declare -x %s\n", env_vars[i]);
		}
		i++;
	}
}

/**
 * @brief Handles the case where `export` command is called without any further arguments.
 *
 * In this case, all env variables (even unassigned ones) are printed in alphabetical order, with
 * prefix 'declare -x'
 *
 * @return int Returns 0 on success, or 1 if an error occurs.
 */
static int	handle_export_wo_args(t_command *cmd, t_data *data)
{
	int		env_count;
	char	**sorted_env_vars;
	int		i;

	(void)cmd;

	// Count the number of environment variables
	env_count = 0;
	while (data->env_vars[env_count] != NULL)
		env_count++;

	// Allocate memory for a temporary array to hold the sorted environment variables
	sorted_env_vars = malloc((env_count + 1) * sizeof(char *));
	if (!sorted_env_vars)
		return (1); // Handle memory allocation failure

	// Copy the environment variables into the sorted array
	i = 0;
	while (i < env_count)
	{
		sorted_env_vars[i] = data->env_vars[i];
		i++;
	}
	sorted_env_vars[env_count] = NULL;

	// Sort the environment variables alphabetically
	sort_env_vars(sorted_env_vars, env_count);

	// Print the sorted environment variables with the correct format
	print_sorted_env_vars(sorted_env_vars);

	// Free the temporary sorted array
	free(sorted_env_vars);

	return (0); // Return success
}

/**
 * @brief Handles the `export` builtin command.
 *
 * This function exports a variable, i.e. adds or updates a variable in the environment variables.
 *
 * @param varname_value The string in the format "varname=value".
 * @param data->env_vars The environment variables array.
 * @return int Returns 0 on success, or 1 if an error occurs.
 */
int	builtin_export(t_command *cmd, t_data *data)
{
	int		i;
	int		arg;
	char	*equal_sign;
	char	*varname;
	//char	*value;
	char	*varname_value;
	int		encountered_invalid_varname = 0;
	
	if (cmd->args[1] == NULL)	
		return (handle_export_wo_args(cmd, data));

	// Process each argument provided in the export command
	arg = 1;
	while (cmd->args[arg] != NULL)
	{
		varname_value = cmd->args[arg];
		
 		/*
		// printing stuff (just debugging)
    	i = 0;
		while (data->env_vars[i] != NULL)
		{
			printf("%d: %s\n", i, data->env_vars[i]);
		    i++;
		}
		printf("%d: %s\n", i, data->env_vars[i]);
		printf("nr of env vars, beginning: %d\n", i);
		*/

		// Find the '=' sign in the argument
		equal_sign = ft_strchr(varname_value, '=');
		if (equal_sign)
		{
			// Split the input into variable name and value
			varname = ft_substr(varname_value, 0, equal_sign - varname_value); // Get the part before '='
			//value = equal_sign + 1; // Get the part after '=': I actually never need to use it
		}
		else
			varname = strdup(varname_value);

		// Check if the variable name is valid
		if (!is_valid_env_var_name(varname))
		{
			fprintf(stderr, "export: `%s`: not a valid identifier\n", varname);
			free(varname);
			encountered_invalid_varname = 1; // set flag to 1 and return exit code 1 at the end of the function
			arg++;
			continue;  // Skip invalid argument and move to the next one
		}

		// Search for the variable in the environment list
		i = 0;
		while (data->env_vars[i] != NULL)
		{
			// Compare the varname with the environment variable before '=' 
			// (or for declared but unassigned env_var (e.g. with "export TEST", varname_value is equal to TEST):
			// compare the varname_value with the environment variable before \0)
			if (ft_strncmp(data->env_vars[i], varname, ft_strlen(varname)) == 0 && 
				(data->env_vars[i][ft_strlen(varname)] == '=' || data->env_vars[i][ft_strlen(varname)] == '\0'))
				break ;
			i++;
		}
		// If found, update its value
		if (data->env_vars[i] != NULL)
		{
			// Free the old value before assigning a new one
			free(data->env_vars[i]);
			data->env_vars[i] = strdup(varname_value);  // Duplicate the string into data->env_vars[i]
			if (!data->env_vars[i])
    			return (1);  // IMPLEMENT CLEAN UP Handle memory allocation failure
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
		}
		// Free the temporary varname allocation
		free(varname);
		// Move to the next argument in the export command
		arg++;
 		/*
		// printing stuff (just debugging)
    	i = 0;
		while (data->env_vars[i] != NULL)
		{
			printf("%d: %s\n", i, data->env_vars[i]);
	    	i++;
		}
		printf("%d: %s\n", i, data->env_vars[i]);
		printf("nr of env vars, end      : %d\n", i);
		*/
	}
	if (encountered_invalid_varname == 1)
		return (1);
	return (0);
}