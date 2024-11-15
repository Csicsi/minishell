/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expansion_bonus.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcsicsak <dcsicsak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/12 13:26:05 by dcsicsak          #+#    #+#             */
/*   Updated: 2024/11/15 18:09:05 by dcsicsak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell_bonus.h"

static void	expand_env_value(char *result, int *i, t_expand_context *context)
{
	char	*env_value;
	char	**matches;

	env_value = ft_getenv(ft_strndup(context->cursor, context->len),
			context->data->env_vars);
	if (!env_value)
		return ;
	if (ft_strchr(env_value, '*'))
	{
		matches = get_matching_files(env_value);
		if (matches && matches[0])
			append_matches_to_result(result, i, matches);
	}
	else
	{
		ft_strcpy(&result[*i], env_value);
		*i += ft_strlen(env_value);
	}
}

static int	expand_var_into_buffer(char *result, int *i,
	char *cursor, t_data *data)
{
	int					len;
	char				*status_str;
	t_expand_context	context;

	len = 0;
	if (*cursor == '?')
	{
		status_str = ft_itoa(data->last_exit_status);
		ft_strcpy(&result[*i], status_str);
		*i += ft_strlen(status_str);
		free(status_str);
		return (1);
	}
	while (ft_isalnum(cursor[len]) || cursor[len] == '_')
		len++;
	if (len > 0)
	{
		context.cursor = cursor;
		context.len = len;
		context.data = data;
		expand_env_value(result, i, &context);
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
