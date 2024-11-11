#include "../includes/minishell.h"

static void	remove_empty_or_space_tokens(t_data *data,
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
		{
			next_token->word++;
		}
		free((*current)->value);
		free(*current);
		*current = next_token;
	}
}

static void	create_new_tokens(t_token **current,
	char **split_words, int *word_index)
{
	t_token	*new_token;
	int		i;

	i = 1;
	while (split_words[i])
	{
		new_token = malloc(sizeof(t_token));
		if (!new_token)
			return ;
		new_token->value = ft_strdup(split_words[i]);
		if (!new_token->value)
			return (free(new_token));
		new_token->old_value = NULL;
		new_token->type = TOKEN_WORD;
		new_token->word = ++(*word_index);
		new_token->is_expanded = false;
		new_token->next = (*current)->next;
		(*current)->next = new_token;
		*current = new_token;
		i++;
	}
	i = 0;
	while (split_words[i])
		free(split_words[i++]);
	free(split_words);
}

static bool	initialize_split_token(t_token *current,
	int *original_word, int *increment)
{
	char	**split_words;
	int		word_index;
	bool	starts_with_space;

	word_index = *original_word;
	starts_with_space = (current->value[0] == ' ');
	split_words = ft_split(current->value, ' ');
	if (!split_words)
		return (starts_with_space);
	free(current->value);
	current->value = ft_strdup(split_words[0]);
	if (!current->value)
		return (starts_with_space);
	if (starts_with_space)
	{
		word_index++;
		current->word = word_index;
	}
	create_new_tokens(&current, split_words, &word_index);
	*increment = word_index - *original_word;
	return (starts_with_space);
}

static void	split_expanded_token(t_token *current, int *original_word,
	bool ends_with_space)
{
	t_token	*next_token;
	int		increment;
	int		i;
	bool	starts_with_space;

	starts_with_space = initialize_split_token(current,
			original_word, &increment);
	if (starts_with_space)
		i = 1;
	else
		i = 0;
	while (i < increment + 1 && current)
	{
		current = current->next;
		i++;
	}
	next_token = current;
	if (ends_with_space)
		increment++;
	while (next_token)
	{
		next_token->word += increment;
		next_token = next_token->next;
	}
}

void	handle_single_expanded_token(t_data *data,
	t_token **current, t_token **prev)
{
	int		original_word;
	bool	ends_with_space;

	if (ft_strlen((*current)->value) == 0)
	{
		(*current)->type = TOKEN_EMPTY;
		return ;
	}
	ends_with_space
		= ((*current)->value[ft_strlen((*current)->value) - 1] == ' ');
	remove_empty_or_space_tokens(data, current, prev);
	if (!*current)
		return ;
	original_word = (*current)->word;
	split_expanded_token(*current, &original_word, ends_with_space);
}

void	handle_expanded_tokens(t_data *data)
{
	t_token	*current;
	t_token	*prev;

	current = data->tokens;
	prev = NULL;
	while (current)
	{
		if (current->is_expanded)
			handle_single_expanded_token(data, &current, &prev);
		prev = current;
		current = current->next;
	}
}
