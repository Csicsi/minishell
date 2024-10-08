#include "../includes/minishell.h"

/**
 * @brief Handles the `unset` builtin command.
 *
 * This function unsets (i.e. removes) an environment variable.
 *
 * @param cmd
 * @param data->env_vars The environment variables array.
 * @return int Returns 0 on success, or 1 if an error occurs.
 */

 int	builtin_unset(t_command *cmd, t_data *data)
{
    int		i;
	int		j;
	int		arg;

    // Loop through all args passed to `unset`
	arg = 1;
	while (cmd->args[arg] != NULL)
	{
        // Search for the environment variable in data->env_var
		i = 0;
        while (data->env_vars[i] != NULL)
        {
            // Compare the variable name
            if (ft_strncmp(data->env_vars[i], cmd->args[arg], ft_strlen(cmd->args[arg])) == 0
                && (data->env_vars[i][ft_strlen(cmd->args[arg])] == '=' || data->env_vars[i][ft_strlen(cmd->args[arg])] == '\0'))
            {
                // Free the memory for this environment variable
                free(data->env_vars[i]);
                // Shift remaining variables up
				j = i;
                while (data->env_vars[j] != NULL)
				{
                    data->env_vars[j] = data->env_vars[j + 1];
					j++;
				}
                break;  // Exit the inner loop after removal
            }
			i++;
        }
		arg++;
    }
    return (0);
}
