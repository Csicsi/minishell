/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krabitsc <krabitsc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/19 17:03:09 by krabitsc          #+#    #+#             */
/*   Updated: 2024/10/19 17:03:12 by krabitsc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

int	builtin_env(t_data *data)
{
	int		i;
	char	**envp;

	envp = data->env_vars;
	if (!data || !data->env_vars)
		return (1);
	i = 0;
	while (envp[i] != NULL)
	{
		if (ft_strchr(envp[i], '=') != NULL)
		{
			printf("%s\n", envp[i]);
		}
		i++;
	}
	return (0);
}
