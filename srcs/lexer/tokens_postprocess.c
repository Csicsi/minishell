#include "../includes/minishell.h"

static void	remove_all_space_token(t_data *data,
	t_token **current, t_token **prev)
{
	t_token	*next_token;

	next_token = (*current)->next;
	if (*prev)
		(*prev)->next = next_token;
	else
		data->tokens = next_token;
	free((*current)->value);
	free(*current);
	*current = next_token;
	if (next_token)
		next_token->word++;
}

static void	add_split_tokens(t_token **current,
	char **split_words, int *word_index)
{
	int		i;
	t_token	*new_token;

	i = 1;
	while (split_words[i])
	{
		new_token = create_token(TOKEN_WORD, ++(*word_index));
		new_token->value = ft_strdup(split_words[i]);
		new_token->next = (*current)->next;
		(*current)->next = new_token;
		*current = new_token;
		i++;
	}
}

static void	split_expanded_token(t_token **current)
{
	char	**split_words;
	int		word_index;
	int		i;

	split_words = ft_split((*current)->value, ' ');
	if (!split_words)
		return ;
	free((*current)->value);
	(*current)->value = ft_strdup(split_words[0]);
	word_index = (*current)->word;
	add_split_tokens(current, split_words, &word_index);
	i = 0;
	while (split_words[i])
		free(split_words[i++]);
	free(split_words);
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
		{
			if (is_all_spaces(current->value))
			{
				remove_all_space_token(data, &current, &prev);
				continue ;
			}
			split_expanded_token(&current);
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
