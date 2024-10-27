/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcsicsak <dcsicsak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/19 11:00:53 by krabitsc          #+#    #+#             */
/*   Updated: 2024/10/26 08:22:02 by dcsicsak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

static void	remove_env_var(char **env_vars, int index)
{
	int	j;

	j = index;
	free(env_vars[index]);
	while (env_vars && env_vars[j] != NULL)
	{
		env_vars[j] = env_vars[j + 1];
		j++;
	}
}

static int	find_env_var_index(char **env_vars, const char *arg)
{
	int	i;

	i = 0;
	while (env_vars && env_vars[i] != NULL)
	{
		if (ft_strncmp(env_vars[i], arg, ft_strlen(arg)) == 0
			&& (env_vars[i][ft_strlen(arg)] == '='
			|| env_vars[i][ft_strlen(arg)] == '\0'))
		{
			return (i);
		}
		i++;
	}
	return (-1);
}

int	builtin_unset(t_cmd *cmd, t_data *data)
{
	int	arg;
	int	index;

	arg = 1;
	while (cmd->args && cmd->args[arg] != NULL)
	{
		index = find_env_var_index(data->env_vars, cmd->args[arg]);
		if (index != -1)
		{
			remove_env_var(data->env_vars, index);
		}
		arg++;
	}
	return (0);
}
