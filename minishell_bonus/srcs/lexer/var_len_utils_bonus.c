/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   var_len_utils_bonus.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcsicsak <dcsicsak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/18 09:54:15 by dcsicsak          #+#    #+#             */
/*   Updated: 2024/11/18 09:54:28 by dcsicsak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell_bonus.h"

int	get_status_var_length(int last_exit_status, int *skip_len)
{
	char	*status_str;
	int		status_length;

	status_str = ft_itoa(last_exit_status);
	status_length = ft_strlen(status_str);
	free(status_str);
	*skip_len = 1;
	return (status_length);
}

int	get_pid_var_length(int *skip_len, t_data *data)
{
	char	*pid_str;
	int		pid_length;

	pid_str = ft_itoa(data->pid);
	pid_length = ft_strlen(pid_str);
	free(pid_str);
	*skip_len = 1;
	return (pid_length);
}

int	get_var_name_length(char *cursor, int *skip_len)
{
	int	len;

	len = 0;
	while (cursor[len] && (ft_isalnum(cursor[len]) || cursor[len] == '_'))
		len++;
	*skip_len = len;
	return (len);
}
