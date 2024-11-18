/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expansion_utils_bonus.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcsicsak <dcsicsak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/18 07:32:15 by dcsicsak          #+#    #+#             */
/*   Updated: 2024/11/18 07:39:24 by dcsicsak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell_bonus.h"

int	expand_special_var(char *result, int *i, char *cursor,
	t_data *data)
{
	char	*status_str;

	if (*cursor == '?')
	{
		status_str = ft_itoa(data->last_exit_status);
		if (status_str)
		{
			ft_strcpy(&result[*i], status_str);
			*i += ft_strlen(status_str);
			free(status_str);
		}
		return (1);
	}
	if (*cursor == '$')
	{
		status_str = ft_itoa(getpid());
		if (status_str)
		{
			ft_strcpy(&result[*i], status_str);
			*i += ft_strlen(status_str);
			free(status_str);
		}
		return (2);
	}
	return (0);
}

int	expand_normal_var(char *result, int *i, char *cursor, t_data *data)
{
	int		len;
	char	*env_value;
	char	*key;

	len = 0;
	while (ft_isalnum(cursor[len]) || cursor[len] == '_')
		len++;
	if (len > 0)
	{
		key = ft_strndup(cursor, len);
		if (!key)
			return (0);
		env_value = ft_getenv(key, data->env_vars);
		if (env_value)
		{
			ft_strcpy(&result[*i], env_value);
			*i += ft_strlen(env_value);
		}
	}
	return (len);
}

int	expand_var_into_buffer(char *result, int *i, char *cursor,
	t_data *data)
{
	int	special_len;

	special_len = expand_special_var(result, i, cursor, data);
	if (special_len > 0)
		return (special_len);
	return (expand_normal_var(result, i, cursor, data));
}
