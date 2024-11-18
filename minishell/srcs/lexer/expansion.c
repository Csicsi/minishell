/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expansion.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcsicsak <dcsicsak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/12 13:26:05 by dcsicsak          #+#    #+#             */
/*   Updated: 2024/11/18 08:44:06 by dcsicsak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

static void	expand_env_vars_into_buffer(char *result,
	char *cursor, t_data *data)
{
	int	i;
	int	skip_len;

	i = 0;
	while (*cursor)
	{
		if (*cursor == '$' && *(cursor + 1) == '/')
		{
			cursor += 2;
			skip_len = expand_var_into_buffer(result, &i, "$/", data);
		}
		if (*cursor == '$' && *(cursor + 1) == '$')
		{
			cursor += 2;
			skip_len = expand_var_into_buffer(result, &i, "$$", data);
		}
		else if (*cursor == '$' && *(cursor + 1))
		{
			skip_len = expand_var_into_buffer(result, &i, ++cursor, data);
			cursor += skip_len;
		}
		else
			result[i++] = *cursor++;
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
