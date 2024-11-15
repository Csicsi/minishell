/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_utils_bonus.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcsicsak <dcsicsak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/12 13:24:43 by dcsicsak          #+#    #+#             */
/*   Updated: 2024/11/15 16:13:16 by dcsicsak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell_bonus.h"

static void	sort_env_vars(char **env_vars, int count)
{
	int		i;
	int		j;
	char	*temp;

	i = 0;
	while (i < count - 1)
	{
		j = i + 1;
		while (j < count)
		{
			if (ft_strcmp(env_vars[i], env_vars[j]) > 0)
			{
				temp = env_vars[i];
				env_vars[i] = env_vars[j];
				env_vars[j] = temp;
			}
			j++;
		}
		i++;
	}
}

static void	print_sorted_env_vars(char **env_vars)
{
	int		i;
	char	*equal_sign;

	i = 0;
	while (env_vars[i] != NULL)
	{
		if (ft_strncmp(env_vars[i], "_=", 2) == 0)
		{
			i++;
			continue ;
		}
		equal_sign = ft_strchr(env_vars[i], '=');
		if (equal_sign)
		{
			printf("declare -x ");
			printf("%.*s", (int)(equal_sign - env_vars[i]), env_vars[i]);
			printf("=\"%s\"\n", equal_sign + 1);
		}
		else
		{
			printf("declare -x %s\n", env_vars[i]);
		}
		i++;
	}
}

int	handle_export_wo_args(t_cmd *cmd, t_data *data)
{
	int		env_count;
	char	**sorted_env_vars;
	int		i;

	(void)cmd;
	env_count = 0;
	while (data->env_vars[env_count] != NULL)
		env_count++;
	sorted_env_vars = malloc((env_count + 1) * sizeof(char *));
	if (!sorted_env_vars)
		return (1);
	i = 0;
	while (i < env_count)
	{
		sorted_env_vars[i] = data->env_vars[i];
		i++;
	}
	sorted_env_vars[env_count] = NULL;
	sort_env_vars(sorted_env_vars, env_count);
	print_sorted_env_vars(sorted_env_vars);
	free(sorted_env_vars);
	return (0);
}

int	is_valid_env_var_name(const char *name)
{
	if (!ft_isalpha(*name) && *name != '_')
		return (0);
	while (*name)
	{
		if (!ft_isalnum(*name) && *name != '_')
			return (0);
		name++;
	}
	return (1);
}
