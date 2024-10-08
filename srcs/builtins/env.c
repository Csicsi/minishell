#include "../includes/minishell.h"

/**
 * @brief Handles the `env` builtin command.
 *
 * This function prints all the environment variables currently set in the shell to the terminal.
 * (apart for ones that have been added with export but not been assigned a value:
 * e.g., "export VAR_WO_ASSIGN", creates this particular environment var, but cmd "env" does not
 * print it to the terminal (whereas "export" does))
 *
 * @param t_data *data
 * @return int Returns 0 on success, or 1 if an error occurs.
 */
int	builtin_env(t_data *data)
{
    int		i;
	char	**envp = data->env_vars;

	if (!data || !data->env_vars)
    	return (1);  // Return 1 to indicate an error

    // Loop through the environment variables
    i = 0;
	while (envp[i] != NULL)
    {
        // Check if the environment variable contains an equal sign ('=')
        if (ft_strchr(envp[i], '=') != NULL) // Only print if '=' is present
        {
            printf("%s\n", envp[i]);  // Print the variable in the form of KEY=VALUE
        }
        i++;
    }
    return 0;  // Return 0 to indicate success
}
