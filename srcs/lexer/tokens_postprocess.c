#include "../includes/minishell.h"

void	remove_empty_or_space_tokens(t_data *data,
	t_token **current, t_token **prev)
{
	t_token	*next_token;

	if (ft_strcmp((*current)->value, "") == 0
		|| is_all_spaces((*current)->value))
	{
		next_token = (*current)->next;
		if (*prev)
			(*prev)->next = next_token;
		else
			data->tokens = next_token;
		if (ft_strcmp((*current)->value, "") != 0
			&& is_all_spaces((*current)->value) && next_token)
			next_token->word++;
		free((*current)->value);
		free(*current);
		*current = next_token;
	}
}

void	split_expanded_token(t_token *current, int *original_word)
{
	char	**split_words;
	t_token	*new_token;
	t_token	*next_token;
	int		i;
	int		increment;
	int		word_index;
	bool	ends_with_space;
	bool	starts_with_space;

	i = 1;
	word_index = *original_word;
	ends_with_space = (current->value[ft_strlen(current->value) - 1] == ' ');
	starts_with_space = (current->value[0] == ' ');
	split_words = ft_split(current->value, ' ');
	if (!split_words)
		return ;
	free(current->value);
	current->value = ft_strdup(split_words[0]);
	if (!current->value)
		return ;
	if (starts_with_space)
	{
		word_index++;
		current->word = word_index;
	}
	while (split_words[i])
	{
		new_token = malloc(sizeof(t_token));
		if (!new_token)
			return ;
		new_token->value = ft_strdup(split_words[i]);
		if (!new_token->value)
		{
			free(new_token);
			return ;
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
	{
		free(split_words[i]);
		i++;
	}
	free(split_words);
	increment = word_index - *original_word;
	if (ends_with_space)
		increment++;
	next_token = current->next;
	while (next_token)
	{
		next_token->word += increment;
		next_token = next_token->next;
	}
}

void	handle_expanded_tokens(t_data *data)
{
	t_token	*current;
	t_token	*prev;
	int		original_word;

	current = data->tokens;
	prev = NULL;
	while (current)
	{
		if (current->is_expanded)
		{
			remove_empty_or_space_tokens(data, &current, &prev);
			if (!current)
				continue ;
			original_word = current->word;
			split_expanded_token(current, &original_word);
		}
		prev = current;
		current = current->next;
	}
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
