/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd_utils2.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcsicsak <dcsicsak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/20 10:55:16 by krabitsc          #+#    #+#             */
/*   Updated: 2024/11/15 17:35:04 by dcsicsak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

static char	*construct_varname_value(const char *varname, const char *value)
{
	int		len_varname;
	int		len_value;
	char	*varname_value;

	len_varname = ft_strlen(varname);
	len_value = ft_strlen(value);
	varname_value = malloc(len_varname + len_value + 2);
	if (!varname_value)
		return (NULL);
	ft_strlcpy(varname_value, varname, len_varname + 1);
	varname_value[len_varname] = '=';
	ft_strlcpy(varname_value + len_varname + 1, value, len_value + 1);
	return (varname_value);
}

static int	find_env_var_index(const char *varname, t_data *data)
{
	int	i;

	i = 0;
	while (data->env_vars && data->env_vars[i] != NULL)
	{
		if (ft_strncmp(data->env_vars[i], varname, ft_strlen(varname)) == 0
			&& (data->env_vars[i][ft_strlen(varname)] == '='
			|| data->env_vars[i][ft_strlen(varname)] == '\0'))
			return (i);
		i++;
	}
	return (-1);
}

static int	update_existing_env_var(int i, char *varname_value, t_data *data)
{
	free(data->env_vars[i]);
	data->env_vars[i] = ft_strdup(varname_value);
	if (!data->env_vars[i])
		return (1);
	return (0);
}

static int	add_new_env_var(char *varname_value, t_data *data)
{
	int	env_count;

	env_count = 0;
	while (data->env_vars && data->env_vars[env_count] != NULL)
		env_count++;
	data->env_vars = ft_realloc(data->env_vars,
			(env_count + 1) * sizeof(char *), (env_count + 2) * sizeof(char *));
	if (!data->env_vars)
		return (1);
	data->env_vars[env_count] = ft_strdup(varname_value);
	if (!data->env_vars[env_count])
		return (1);
	data->env_vars[env_count + 1] = NULL;
	return (0);
}

int	ft_setenv(const char *varname, const char *value, t_data *data)
{
	char	*varname_value;
	int		index;

	varname_value = construct_varname_value(varname, value);
	if (!varname_value)
		return (1);
	index = find_env_var_index(varname, data);
	if (index >= 0)
	{
		if (update_existing_env_var(index, varname_value, data) != 0)
			return (free(varname_value), 1);
	}
	else
	{
		if (add_new_env_var(varname_value, data) != 0)
			return (free(varname_value), 1);
	}
	free(varname_value);
	return (0);
}
