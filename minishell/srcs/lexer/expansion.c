/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expansion.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcsicsak <dcsicsak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/12 13:26:05 by dcsicsak          #+#    #+#             */
/*   Updated: 2024/11/18 09:16:14 by dcsicsak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

static int	expand_special_var(char *result, int *i, char *cursor, t_data *data)
{
	char	*status_str;
	char	*pid_str;

	if (*cursor == '?')
	{
		status_str = ft_itoa(data->last_exit_status);
		ft_strcpy(&result[*i], status_str);
		*i += ft_strlen(status_str);
		free(status_str);
		return (1);
	}
	if (*cursor == '$')
	{
		pid_str = ft_itoa(data->pid);
		ft_strcpy(&result[*i], pid_str);
		*i += ft_strlen(pid_str);
		free(pid_str);
		return (1);
	}
	return (0);
}

static int	expand_var_into_buffer(char *result,
	int *i, char *cursor, t_data *data)
{
	int		len;
	char	*env_value;

	len = 0;
	if (expand_special_var(result, i, cursor, data))
		return (1);
	while (ft_isalnum(cursor[len]) || cursor[len] == '_')
		len++;
	if (len > 0)
	{
		env_value = ft_getenv(ft_strndup(cursor, len), data->env_vars);
		if (env_value)
		{
			ft_strcpy(&result[*i], env_value);
			*i += ft_strlen(env_value);
		}
	}
	return (len);
}

static void	expand_env_vars_into_buffer(char
	*result, char *cursor, t_data *data)
{
	int	i;
	int	skip_len;

	i = 0;
	while (*cursor)
	{
		if (*cursor == '$' && *(cursor + 1) == '/')
		{
			result[i++] = *cursor++;
			result[i++] = *cursor++;
		}
		else if (*cursor == '$' && *(cursor + 1))
		{
			cursor++;
			skip_len = expand_var_into_buffer(result, &i, cursor, data);
			cursor += skip_len;
		}
		else
		{
			result[i++] = *cursor++;
		}
	}
	result[i] = '\0';
}

char	*expand_env_var(char *cursor, int last_exit_status, t_data *data)
{
	int		final_length;
	char	*result;

	final_length = calculate_expanded_length(cursor, last_exit_status, data);
	result = malloc(final_length + 1);
	if (!result)
		return (NULL);
	expand_env_vars_into_buffer(result, cursor, data);
	return (result);
}

char	*expand_env_var_in_str(char **ptr_to_cursor,
	int last_exit_status, t_data *data)
{
	char	*cursor;
	char	*result;
	int		final_length;

	cursor = *ptr_to_cursor;
	final_length = calculate_expanded_length(cursor, last_exit_status, data);
	result = malloc(final_length + 1);
	if (!result)
		return (NULL);
	expand_env_vars_into_buffer(result, cursor, data);
	free(*ptr_to_cursor);
	*ptr_to_cursor = result;
	return (*ptr_to_cursor);
}
