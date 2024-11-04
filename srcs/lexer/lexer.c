#include "../includes/minishell.h"

static char	*handle_unquoted_word(char *cursor,
	t_token *new_token, bool *in_heredoc, t_data *data)
{
	cursor = extract_unquoted_word(cursor, new_token);
	if (!new_token->value)
		return (NULL);
	if (!*in_heredoc && ft_strchr(new_token->value, '$'))
	{
		if (!handle_env_variables(new_token, data))
		{
			free(new_token->value);
			return (NULL);
		}
	}
	new_token->type = TOKEN_WORD;
	if (*in_heredoc)
		*in_heredoc = 0;
	return (cursor);
}

static char	*process_token(char *cursor, t_token *new_token, t_data *data)
{
	char	*temp;

	if (*cursor == '<' && *(cursor + 1) == '<')
	{
		cursor = handle_operator_or_quote(cursor, new_token, data);
		temp = cursor;
		while (*temp == ' ')
			temp++;
		if (*temp == '\'')
			data->heredoc_single_quote = true;
	}
	else if (*cursor == '"' || *cursor == '\'' || *cursor == '>'
		|| *cursor == '<' || *cursor == '|' || *cursor == '&')
		cursor = handle_operator_or_quote(cursor, new_token, data);
	else
		cursor = handle_unquoted_word(cursor,
				new_token, &data->in_heredoc, data);
	return (cursor);
}

static char	*create_and_add_token(char *cursor, t_token **token_list,
	t_data *data)
{
	t_token	*new_token;
	t_token	*current;

	current = *token_list;
	new_token = create_token(0, data->word_index);
	if (!new_token)
		return (NULL);
	cursor = process_token(cursor, new_token, data);
	if (!cursor)
		return (free(new_token->value), free(new_token), NULL);
	if (!*token_list)
		*token_list = new_token;
	else
	{
		while (current->next)
			current = current->next;
		current->next = new_token;
	}
	if (ft_isspace(*cursor) || *cursor == '|' || *cursor == '&'
		|| *cursor == '>' || *cursor == '<')
		data->word_index++;
	return (cursor);
}

static void	join_tokens_in_same_word(t_data *data)
{
	t_token	*current;
	t_token	*next_token;
	char	*new_value;

	current = data->tokens;
	while (current && current->next)
	{
		if (current->word == current->next->word)
		{
			next_token = current->next;
			new_value = ft_strjoin(current->value, next_token->value);
			free(current->value);
			current->value = new_value;
			current->next = next_token->next;
			free(next_token->value);
			free(next_token);
		}
		else
			current = current->next;
	}
}

int	lexer(t_data *data)
{
	char	*cursor;

	cursor = data->input;
	data->word_index = 0;
	data->in_heredoc = false;
	data->tokens = NULL;
	while (*cursor != '\0')
	{
		cursor = skip_spaces(cursor);
		if (*cursor == '\0')
			break ;
		cursor = create_and_add_token(cursor, &data->tokens, data);
		if (!cursor)
			return (1);
		cursor = skip_spaces(cursor);
	}
	handle_expanded_tokens(data);
	join_tokens_in_same_word(data);
	return (0);
}
