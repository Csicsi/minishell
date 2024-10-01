/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krabitsc <krabitsc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/15 16:22:47 by dcsicsak          #+#    #+#             */
/*   Updated: 2024/10/01 16:57:18 by krabitsc         ###   ########.fr       */
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

	// Check if the first argument is "-n"
	i = 0;
	no_newline = 0;
	if (cmd->args[i] && ft_strcmp(cmd->args[i], "-n") == 0)
	{
		no_newline = 1;
		i++; // Skip the "-n" argument
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