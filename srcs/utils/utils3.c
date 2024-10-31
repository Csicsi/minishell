#include "../includes/minishell.h"

void	*ft_realloc(void *ptr, size_t old_size, size_t new_size)
{
	void	*new_ptr;

	if (new_size == 0)
	{
		free(ptr);
		return (NULL);
	}
	if (!ptr)
		return (malloc(new_size));
	new_ptr = malloc(new_size);
	if (!new_ptr)
		return (NULL);
	if (new_size < old_size)
		old_size = new_size;
	ft_memcpy(new_ptr, ptr, old_size);
	free(ptr);
	return (new_ptr);
}

int	ft_isdigit_str(const char *str)
{
	int	i;

	i = 0;
	if (!str || str[0] == '\0')
		return (0);
	while (str[i])
	{
		if (!ft_isdigit(str[i]))
			return (0);
		i++;
	}
	return (1);
}

int	check_for_brackets(t_data *data)
{
	int		i;
	char	*input;

	i = 0;
	input = data->input;
	while (input[i])
	{
		if (input[i] == '(' || input[i] == ')')
			return (1);
		i++;
	}
	return (0);
}

bool	check_for_heredoc(t_token *tokens)
{
	t_token	*current;

	current = tokens;
	while (current)
	{
		if (current->type == TOKEN_OPERATOR
			&& ft_strcmp(current->value, "<<") == 0)
			return (true);
		current = current->next;
	}
	return (false);
}

void	ft_free(void **ptr)
{
	if (*ptr)
	{
		free(*ptr);
		*ptr = NULL;
	}
}
