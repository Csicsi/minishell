/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcsicsak <dcsicsak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/15 16:22:47 by dcsicsak          #+#    #+#             */
/*   Updated: 2024/10/03 13:21:18 by dcsicsak         ###   ########.fr       */
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
	bool	no_newline;

	// Check if arguments exist
	if (!cmd || !cmd->args || !cmd->args[0])
		return (1); // Return an error if no arguments

	// Check for the "-n" flag(s)
	i = 1;
	no_newline = false;
	while (cmd->args[i] && cmd->args[i][0] == '-' && cmd->args[i][1] == 'n')
	{
		int j = 2;
		while (cmd->args[i][j] == 'n') // Ensure all characters after "-" are "n"
			j++;
		if (cmd->args[i][j] != '\0') // If there's any non-'n' character, stop checking flags
			break;
		no_newline = true;
		i++;
	}

	// Print the remaining arguments
	while (cmd->args[i])
	{
		printf("%s", cmd->args[i]);
		if (cmd->args[i + 1]) // Add a space between arguments
			printf(" ");
		i++;
	}

	// Print newline unless "-n" was specified
	if (!no_newline)
		printf("\n");

	return (0);
}

