/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quotes_bonus.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: csicsi <csicsi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/12 13:26:16 by dcsicsak          #+#    #+#             */
/*   Updated: 2024/11/12 16:11:39 by csicsi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell_bonus.h"

char	*extract_single_quoted_word(char *cursor, t_token *token)
{
	char	*start;

	start = cursor + 1;
	cursor++;
	while (*cursor && *cursor != '\'')
		cursor++;
	token->value = ft_strndup(start, cursor - start);
	if (*cursor)
		cursor++;
	return (cursor);
}

static int	contains_dollar_space(const char *str)
{
	while (*str)
	{
		if (*str == '$' && *(str + 1) == ' ')
			return (1);
		str++;
	}
	return (0);
}

char	*extract_double_quoted_word(char *cursor,
	t_token *token, t_data *data, int in_heredoc)
{
	int		len;
	char	*temp;
	char	*expanded;
	char	*start;

	start = cursor + 1;
	len = 0;
	while (start[len] && start[len] != '"')
	{
		if (start[len] == '\\' && (start[len + 1] == '"'
				|| start[len + 1] == '$' || start[len + 1] == '\\'))
			len++;
		len++;
	}
	temp = ft_strndup(cursor + 1, len);
	cursor += len + 2;
	if (in_heredoc || contains_dollar_space(temp))
		expanded = ft_strdup(temp);
	else
		expanded = expand_env_var(temp, data->last_exit_status, data);
	if (expanded)
		token->value = ft_strdup(expanded);
	else
		token->value = NULL;
	return (free(expanded), free(temp), cursor);
}
