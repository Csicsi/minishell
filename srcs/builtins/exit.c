/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcsicsak <dcsicsak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/12 13:24:31 by dcsicsak          #+#    #+#             */
/*   Updated: 2024/11/12 13:24:32 by dcsicsak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

int	ft_str_to_num(const char *str, unsigned long long max_value)
{
	unsigned long long	num;

	num = 0;
	while (*str && ft_isdigit((unsigned char)*str))
	{
		if (num > (max_value / 10))
			return (0);
		if (num == (max_value / 10)
			&& (unsigned long long)(*str - '0') > (max_value % 10))
			return (0);
		num = num * 10 + (*str - '0');
		str++;
	}
	while (*str && ft_isspace((unsigned char)*str))
		str++;
	return (*str == '\0');
}

int	ft_strisnum(const char *str)
{
	int					sign;
	unsigned long long	max_value;

	if (!str || !*str)
		return (0);
	while (*str && ft_isspace((unsigned char)*str))
		str++;
	sign = 1;
	if (*str == '+' || *str == '-')
	{
		if (*str == '-')
			sign = -1;
		str++;
	}
	if (!*str || !ft_isdigit((unsigned char)*str))
		return (0);
	if (sign == -1)
		max_value = -(unsigned long long)LLONG_MIN;
	else
		max_value = LLONG_MAX;
	return (ft_str_to_num(str, max_value));
}

int	builtin_exit(t_cmd *cmd, t_data *data, bool print_exit)
{
	data->exit_flag = true;
	if (isatty(STDIN_FILENO) && print_exit)
		ft_fprintf(2, "exit\n");
	if (cmd->args[1] && ft_strisnum(cmd->args[1]) == 0)
	{
		ft_fprintf(2, ": exit: %s: numeric argument required\n",
			cmd->args[1]);
		return (2);
	}
	if (cmd->args[1] && cmd->args[2])
	{
		ft_fprintf(2, ": exit: too many arguments\n");
		return (1);
	}
	if (cmd->args[1])
		data->last_exit_status = ft_atoi(cmd->args[1]);
	data->last_exit_status &= 255;
	return (data->last_exit_status);
}
