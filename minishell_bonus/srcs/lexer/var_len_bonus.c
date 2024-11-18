/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   var_len_bonus.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcsicsak <dcsicsak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/12 13:26:23 by dcsicsak          #+#    #+#             */
/*   Updated: 2024/11/18 08:46:06 by dcsicsak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell_bonus.h"

static int	get_status_var_length(int last_exit_status, int *skip_len)
{
	char	*status_str;
	int		status_length;

	status_str = ft_itoa(last_exit_status);
	status_length = ft_strlen(status_str);
	free(status_str);
	*skip_len = 1;
	return (status_length);
}

static int	get_var_name_length(char *cursor, int *skip_len)
{
	int	len;

	len = 0;
	while (cursor[len] && (ft_isalnum(cursor[len]) || cursor[len] == '_'))
		len++;
	*skip_len = len;
	return (len);
}

static int	get_wildcard_expansion_length(const char *pattern)
{
	char	**matches;
	int		total_length;
	int		i;

	matches = get_matching_files(pattern);
	if (!matches)
		return (0);
	total_length = 0;
	i = 0;
	while (matches[i])
	{
		total_length += ft_strlen(matches[i]);
		if (matches[i + 1])
			total_length++;
		free(matches[i]);
		i++;
	}
	free(matches);
	return (total_length);
}

static int	get_expanded_var_length(char *cursor,
	int last_exit_status, t_data *data, int *skip_len)
{
	int		var_length;
	char	*env_value;

	if (*cursor == '?')
		return (get_status_var_length(last_exit_status, skip_len));
	else if (*cursor == '$')
		return (get_pid_length(skip_len));
	else
	{
		var_length = get_var_name_length(cursor, skip_len);
		if (*skip_len > 0)
		{
			env_value = ft_getenv(ft_strndup(cursor, *skip_len),
					data->env_vars);
			if (env_value)
			{
				if (ft_strchr(env_value, '*'))
					var_length = get_wildcard_expansion_length(env_value);
				else
					var_length = ft_strlen(env_value);
			}
		}
	}
	return (var_length);
}

int	calculate_expanded_length(char *cursor, int last_exit_status, t_data *data)
{
	int	total_length;
	int	skip_len;

	total_length = 0;
	while (*cursor)
	{
		if (*cursor == '$' && *(cursor + 1))
		{
			if (*(cursor + 1) == '/')
				total_length += 1;
			cursor++;
			total_length += get_expanded_var_length(cursor,
					last_exit_status, data, &skip_len);
			cursor += skip_len;
		}
		else
		{
			total_length++;
			cursor++;
		}
	}
	return (total_length);
}
