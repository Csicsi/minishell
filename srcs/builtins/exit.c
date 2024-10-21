/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krabitsc <krabitsc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/19 17:10:13 by krabitsc          #+#    #+#             */
/*   Updated: 2024/10/21 09:21:16 by krabitsc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

int	ft_strisnum(const char *str)
{
	if (!str || !*str)
		return (0);
	if (*str == '+' || *str == '-')
		str++;
	if (!*str)
		return (0);
	while (*str)
	{
		if (!isdigit((unsigned char)*str))
			return (0);
		str++;
	}
	return (1);
}

int	builtin_exit(t_cmd *cmd, t_data *data, bool print_exit)
{
	data->exit_flag = true;
	if (isatty(STDIN_FILENO) && print_exit)
		ft_fprintf(2, "exit\n");
	if (cmd->args[1] && cmd->args[2])
	{
		ft_fprintf(2, "minishell: exit: too many arguments\n");
		return (1);
	}
	if (cmd->args[1] && ft_strisnum(cmd->args[1]) == 0)
	{
		ft_fprintf(2, "minishell: exit: %s: numeric argument required\n",
			cmd->args[1]);
		return (255);
	}
	if (cmd->args[1])
		data->last_exit_status = ft_atoi(cmd->args[1]);
	data->last_exit_status &= 255;
	return (data->last_exit_status);
}
