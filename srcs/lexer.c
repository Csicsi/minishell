#include "../includes/minishell.h"

char	*skip_spaces(char *cursor)
{
	while (*cursor && isspace(*cursor))
		cursor++;
	return (cursor);
}

char	*extract_single_quoted_word(char *cursor, t_token *token)
{
	char	*start;

	start = ++cursor;
	while (*cursor && *cursor != '\'')
		cursor++;
	token->value = ft_strndup(start, cursor - start);
	return (++cursor);
}

int	calculate_expanded_length(char *cursor, int last_exit_status, t_data *data)
{
	int		total_length;
	int		len;
	char	*start;
	char	*var_name;
	char	*env_value;
	char	*exit_status_str;

	total_length = 0;
	while (*cursor)
	{
		if (*cursor == '$')
		{
			start = cursor + 1;
			if (*start == '?')
			{
				exit_status_str = ft_itoa(last_exit_status);
				total_length += ft_strlen(exit_status_str);
				free(exit_status_str);
				cursor += 2;
			}
			else
			{
				len = 0;
				while (isalnum(start[len]) || start[len] == '_')
					len++;
				if (len > 0)
				{
					var_name = ft_strndup(start, len);
					env_value = ft_getenv(var_name, data->env_vars);
					if (env_value)
						total_length += ft_strlen(env_value);
					cursor += len + 1;
				}
				else
				{
					total_length++;
					cursor++;
				}
			}
		}
		else
		{
			total_length++;
			cursor++;
		}
	}
	return (total_length);
}

void	expand_env_vars_into_buffer(char *result, char *cursor, int last_exit_status, t_data *data)
{
	int		i;
	int		len;
	char	*start;
	char	*var_name;
	char	*env_value;
	char	*exit_status_str;

	(void)data;
	i = 0;
	while (*cursor)
	{
		if (*cursor == '$')
		{
			start = cursor + 1;
			if (*start == '?')
			{
				exit_status_str = ft_itoa(last_exit_status);
				ft_strcpy(&result[i], exit_status_str);
				i += ft_strlen(exit_status_str);
				free(exit_status_str);
				cursor += 2;
			}
			else
			{
				len = 0;
				while (isalnum(start[len]) || start[len] == '_')
					len++;
				if (len > 0)
				{
					var_name = ft_strndup(start, len);
					env_value = ft_getenv(var_name, data->env_vars);
					if (env_value)
					{
						ft_strcpy(&result[i], env_value);
						i += ft_strlen(env_value);
					}
					cursor += len + 1;
				}
				else
				{
					result[i++] = *cursor;
					cursor++;
				}
			}
		}
		else
		{
			result[i++] = *cursor;
			cursor++;
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
	expand_env_vars_into_buffer(result, cursor, last_exit_status, data);
	return (result);
}

char	*extract_double_quoted_word(char *cursor, t_token *token, int last_exit_status, t_data *data, int in_heredoc)
{
	int		len;
	int		i;
	char	*start;
	char	*expanded;
	char	*temp;

	start = cursor + 1;
	len = 0;
	while (start[len] && start[len] != '"')
	{
		if (start[len] == '\\' && (start[len + 1] == '"' || start[len + 1] == '$' || start[len + 1] == '\\'))
			len++;
		len++;
	}
	temp = malloc(len + 1);
	if (!temp)
		return (NULL);
	i = 0;
	cursor++;
	while (*cursor && *cursor != '"')
	{
		if (*cursor == '\\' && (*(cursor + 1) == '"' || *(cursor + 1) == '$' || *(cursor + 1) == '\\'))
			temp[i++] = *(++cursor);
		else
			temp[i++] = *cursor;
		cursor++;
	}
	temp[i] = '\0';
	if (!in_heredoc)
		expanded = expand_env_var(temp, last_exit_status, data);
	else
		expanded = ft_strdup(temp);
	free(temp);
	token->value = ft_strdup(expanded);
	free(expanded);
	return (++cursor);
}

char	*check_operator(char *cursor, t_token *token)
{
	if (*cursor == '>' && *(cursor + 1) == '>')
	{
		token->value = ft_strdup(">>");
		return (cursor + 2);
	}
	else if (*cursor == '<' && *(cursor + 1) == '<')
	{
		token->value = ft_strdup("<<");
		return (cursor + 2);
	}
	else if (*cursor == '&' && *(cursor + 1) == '&')
	{
		token->value = ft_strdup("&&");
		return (cursor + 2);
	}
	else if (*cursor == '|' && *(cursor + 1) == '|')
	{
		token->value = ft_strdup("||");
		return (cursor + 2);
	}
	else if (*cursor == '>' || *cursor == '<' || *cursor == '|' || *cursor == '&')
	{
		token->value = ft_strndup(cursor, 1);
		return (cursor + 1);
	}
	return (cursor);
}

char	*expand_env_var_in_str(char **ptr_to_cursor, int last_exit_status, t_data *data)
{
	char	*cursor;
	char	*result;
	int		final_length;

	cursor = *ptr_to_cursor;
	final_length = calculate_expanded_length(cursor, last_exit_status, data);
	result = malloc(final_length + 1);
	if (!result)
		return (NULL);
	expand_env_vars_into_buffer(result, cursor, last_exit_status, data);
	free(*ptr_to_cursor);
	*ptr_to_cursor = ft_strdup(result);
	free(result);
	return (*ptr_to_cursor);
}

int	count_tokens(t_token *tokens)
{
	int	count;

	count = 0;
	while (tokens)
	{
		count++;
		tokens = tokens->next;
	}
	return (count);
}

void	join_tokens_in_same_word(t_data *data)
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

void	print_tokens(t_token *tokens)
{
	t_token	*current;

	current = tokens;
	while (current)
	{
		printf("type: %d, value: %s, word: %d, is_expanded: %d\n", current->type, current->value, current->word, current->is_expanded);
		current = current->next;
	}
}

void	split_expanded_tokens_by_spaces(t_data *data)
{
	t_token	*current;
	t_token	*new_token;
	t_token	*next_token;
	char	**split_words;
	int		i;
	int		word_index;

	current = data->tokens;
	while (current)
	{
		if (current->is_expanded)
		{
			split_words = ft_split(current->value, ' ');
			if (!split_words)
				return;
			free(current->value);
			current->value = ft_strdup(split_words[0]);
			if (!current->value)
				return;
			word_index = current->word;
			i = 1;
			while (split_words[i])
			{
				new_token = malloc(sizeof(t_token));
				if (!new_token)
					return;
				new_token->value = ft_strdup(split_words[i]);
				if (!new_token->value)
				{
					free(new_token);
					return;
				}
				new_token->type = TOKEN_WORD;
				new_token->word = ++word_index;
				new_token->is_expanded = false;
				new_token->next = current->next;
				current->next = new_token;
				current = new_token;
				i++;
			}
			i = 0;
			while (split_words[i])
				free(split_words[i++]);
			free(split_words);
			next_token = current->next;
			while (next_token)
			{
				next_token->word += (word_index - current->word);
				next_token = next_token->next;
			}
		}
		current = current->next;
	}
}

int	lexer(char *input, t_data *data, int last_exit_status)
{
	char	*cursor;
	t_token	*current;
	t_token	*new_token;
	int		word_index;
	int		in_heredoc;
	char	*start;
	int		length;
	char	*expanded;

	cursor = input;
	current = NULL;
	word_index = 0;
	in_heredoc = 0;
	data->tokens = NULL;
	while (*cursor != '\0')
	{
		cursor = skip_spaces(cursor);
		if (*cursor == '\0')
			break ;
		new_token = malloc(sizeof(t_token));
		if (!new_token)
			return (-1);
		new_token->next = NULL;
		new_token->type = 0;
		new_token->value = NULL;
		new_token->word = word_index;
		new_token->is_expanded = false;
		if (*cursor == '<' && *(cursor + 1) == '<')
		{
			new_token->type = TOKEN_OPERATOR;
			cursor = check_operator(cursor, new_token);
			in_heredoc = 1;
			word_index++;
		}
		else if (*cursor == '"')
		{
			cursor = extract_double_quoted_word(cursor, new_token, last_exit_status, data, in_heredoc);
			new_token->type = TOKEN_WORD;
		}
		else if (*cursor == '\'')
		{
			cursor = extract_single_quoted_word(cursor, new_token);
			new_token->type = TOKEN_WORD;
		}
		else if (*cursor == '>' || *cursor == '<' || *cursor == '|' || *cursor == '&')
		{
			new_token->type = TOKEN_OPERATOR;
			cursor = check_operator(cursor, new_token);
			word_index++;
		}
		else
		{
			start = cursor;
			length = 0;
			while (!isspace(*cursor) && *cursor != '|' && *cursor != '&'
				&& *cursor != '>' && *cursor != '<' && *cursor != '\0'
				&& *cursor != '"' && *cursor != '\'')
			{
				length++;
				cursor++;
			}
			new_token->value = ft_strndup(start, length);
			if (!new_token->value)
				return (-1);
			if (!in_heredoc && ft_strchr(new_token->value, '$'))
			{
				expanded = expand_env_var_in_str(&new_token->value, last_exit_status, data);
				if (!expanded)
				{
					free(new_token->value);
					free(new_token);
					return (-1);
				}
				new_token->is_expanded = true;
				new_token->value = expanded;
			}
			new_token->type = TOKEN_WORD;
			if (in_heredoc)
				in_heredoc = 0;
		}
		if (!data->tokens)
			data->tokens = new_token;
		else
			current->next = new_token;
		current = new_token;
		if (isspace(*cursor) || *cursor == '|' || *cursor == '&' || *cursor == '>' || *cursor == '<')
			word_index++;
		cursor = skip_spaces(cursor);
	}
	split_expanded_tokens_by_spaces(data);
	//print_tokens(data->tokens);
	join_tokens_in_same_word(data);
	return (0);
}
