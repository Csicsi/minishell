/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcsicsak <dcsicsak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/19 17:01:43 by krabitsc          #+#    #+#             */
/*   Updated: 2024/11/11 12:29:26 by dcsicsak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

static int	check_no_newline_flag(char **args, bool *no_newline)
{
	int	i;
	int	j;

	i = 1;
	*no_newline = false;
	while (args[i] && args[i][0] == '-' && args[i][1] == 'n')
	{
		j = 2;
		while (args[i][j] == 'n')
			j++;
		if (args[i][j] != '\0')
			break ;
		*no_newline = true;
		i++;
	}
	return (i);
}

static void	print_args(char **args, int i, bool no_newline)
{
	while (args[i])
	{
		printf("%s", args[i]);
		if (args[i + 1])
			printf(" ");
		i++;
	}
	if (!no_newline)
		printf("\n");
}

int	builtin_echo(t_cmd *cmd)
{
	int		i;
	bool	no_newline;

	if (!cmd || !cmd->args || !cmd->args[0])
		return (1);
	i = check_no_newline_flag(cmd->args, &no_newline);
	print_args(cmd->args, i, no_newline);
	return (0);
}
