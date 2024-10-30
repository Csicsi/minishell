#include "../includes/minishell.h"

char	*handle_unquoted_word(char *cursor,
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

char	*process_token(char *cursor, t_token *new_token, t_data *data)
{
	if (*cursor == '<' && *(cursor + 1) == '<')
		cursor = handle_operator_or_quote(cursor, new_token, data);
	else if (*cursor == '"' || *cursor == '\'' || *cursor == '>'
		|| *cursor == '<' || *cursor == '|' || *cursor == '&')
		cursor = handle_operator_or_quote(cursor, new_token, data);
	else
		cursor = handle_unquoted_word(cursor,
				new_token, &data->in_heredoc, data);
	return (cursor);
}

char	*create_and_add_token(char *cursor, t_token **token_list, t_data *data)
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
	if (isspace(*cursor) || *cursor == '|' || *cursor == '&'
		|| *cursor == '>' || *cursor == '<')
		data->word_index++;
	return (cursor);
}

void	print_tokens(t_token *token_list)
{
	t_token	*current = token_list;

	while (current)
	{
		printf("Token:\n");
		printf("  Type: %d\n", current->type);
		printf("  Value: %s\n", current->value ? current->value : "(null)");
		printf("  Word Index: %d\n", current->word);
		printf("  Is Expanded: %s\n", current->is_expanded ? "true" : "false");
		printf("  Is Wildcard: %s\n", current->is_wildcard ? "true" : "false");
		printf("  Next: %s\n\n", current->next ? "Exists" : "NULL");
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
	//print_tokens(data->tokens);
	handle_expanded_tokens(data);
	//print_tokens(data->tokens);
	join_tokens_in_same_word(data);
	//print_tokens(data->tokens);
	return (0);
}
