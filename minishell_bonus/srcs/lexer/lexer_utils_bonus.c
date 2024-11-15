/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils_bonus.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcsicsak <dcsicsak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/12 13:26:10 by dcsicsak          #+#    #+#             */
/*   Updated: 2024/11/15 16:17:48 by dcsicsak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell_bonus.h"

t_token	*create_token(int type, int word_index)
{
	t_token	*new_token;

	new_token = malloc(sizeof(t_token));
	if (!new_token)
		return (NULL);
	new_token->type = type;
	new_token->word = word_index;
	new_token->value = NULL;
	new_token->old_value = NULL;
	new_token->is_expanded = false;
	new_token->is_wildcard = false;
	new_token->next = NULL;
	return (new_token);
}

static char	*check_operator(char *cursor, t_token *token)
{
	static const char	*operators[]
		= {"&&", "||", ">>", "<<", ">", "<", "|", "(", ")"};
	int					i;

	i = 0;
	while (i < 9)
	{
		if (!ft_strncmp(cursor, operators[i], ft_strlen(operators[i])))
		{
			token->value = ft_strdup(operators[i]);
			if (ft_strcmp(token->value, "(") == 0)
				token->type = TOKEN_OPEN;
			else if (ft_strcmp(token->value, ")") == 0)
				token->type = TOKEN_CLOSE;
			else
				token->type = TOKEN_OPERATOR;
			return (cursor + ft_strlen(operators[i]));
		}
		i++;
	}
	return (cursor);
}

char	*handle_operator_or_quote(char *cursor,
	t_token *new_token, t_data *data)
{
	if (*cursor == '<' && *(cursor + 1) == '<')
	{
		cursor = check_operator(cursor, new_token);
		data->in_heredoc = true;
		data->word_index++;
	}
	else if (*cursor == '"')
	{
		cursor = extract_double_quoted_word(cursor,
				new_token, data, data->in_heredoc);
		new_token->type = TOKEN_WORD;
	}
	else if (*cursor == '\'')
	{
		cursor = extract_single_quoted_word(cursor, new_token);
		new_token->type = TOKEN_WORD;
	}
	else
	{
		cursor = check_operator(cursor, new_token);
		data->word_index++;
	}
	return (cursor);
}

char	*extract_unquoted_word(char *cursor, t_token *new_token)
{
	char	*start;
	int		length;

	start = cursor;
	length = 0;
	while (!isspace(*cursor) && *cursor != '|' && *cursor != '&'
		&& *cursor != '>' && *cursor != '<' && *cursor != '\0'
		&& *cursor != '"' && *cursor != '\'' && *cursor != '('
		&& *cursor != ')')
	{
		if (*cursor == '*')
			new_token->is_wildcard = true;
		length++;
		cursor++;
	}
	new_token->value = ft_strndup(start, length);
	return (cursor);
}

int	handle_env_variables(t_token *new_token, t_data *data)
{
	char	*expanded;

	expanded = expand_env_var(new_token->value, data->last_exit_status, data);
	if (!expanded)
	{
		free(new_token->value);
		return (0);
	}
	new_token->is_expanded = true;
	new_token->old_value = ft_strdup(new_token->value);
	free(new_token->value);
	new_token->value = expanded;
	return (1);
}
