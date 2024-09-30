/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krabitsc <krabitsc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/15 16:22:47 by dcsicsak          #+#    #+#             */
/*   Updated: 2024/09/30 16:22:19 by krabitsc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/**
 * @brief Handles the `echo` builtin command with support for the `-n` option.
 *
 * @param cmd The command structure containing arguments.
 * @return int Always returns 0 for success.
 */
int	builtin_echo(t_command *cmd)
{
	int		i;
	int		no_newline;
	char	*expanded_arg; // To store the expanded argument
	int		j;             // Cursor for expanding env variables

	// just some debugging printfs. token_count with $STRING inputs is correct, 
	// but somehow they get lost in the args char **. Need further debugging
	// e.g.: minishell> echo $hi hello hola
	// ignores/swallows the $hi input
	i = 0;	
	while (cmd->args[i])
	{
		printf("cmd->args[%d]: %s\n", i, cmd->args[i]);
		char *found = ft_strchr(cmd->args[i], '$');
		if (found != NULL)
    		printf("ft_strchr(cmd->args[%d], '$'): %s\n", i, found);
		else
    		printf("No '$' found in cmd->args[%d].\n", i);

		printf("ft_strchr(cmd->args[%d], '$'): %s\n", i, ft_strchr(cmd->args[i], '$'));
		i++;
	}

	// Check if the first argument is "-n"
	i = 0;
	no_newline = 0;
	if (cmd->args[i] && ft_strcmp(cmd->args[i], "-n") == 0)
	{
		no_newline = 1;
		i++; // Skip the "-n" argument
	}
	
	// Iterate through each argument, expand variables only if needed
	while (cmd->args[i])
	{
		// Check if the argument contains a '$'
		if (ft_strchr(cmd->args[i], '$') != NULL)
		{
			printf("AM I HERE?\n");
			
			// If '$' is present, allocate memory for expanded argument
			expanded_arg = strdup(cmd->args[i]);  // Create a copy of the argument

			// Check and expand environment variables in the argument
			j = 0;
			while (expanded_arg[j] != '\0')
			{
				if (expanded_arg[j] == '$' && expanded_arg[j + 1] != ' ' && expanded_arg[j + 1] != '\0')
				{
					// Update j to skip over the variable name after expansion
					j += expand_env_var(&expanded_arg, &j, &expanded_arg[j]) - &expanded_arg[j];

				}
				else
				{
					j++; // Move cursor if no expansion is needed
				}
			}

			// Print the expanded argument
			printf("%s", expanded_arg);

			// Free the expanded argument after printing
			free(expanded_arg);
		}
		else
		{
			// If no '$', just print the argument as it is
			printf("%s", cmd->args[i]);
		}
		if (cmd->args[i + 1]) // Add a space between arguments
			printf(" ");
		i++;
	}

	// Print newline unless "-n" was specified
	if (!no_newline)
		printf("\n");

	return (0);
}
