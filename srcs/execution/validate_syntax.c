#include "../includes/minishell.h"

static int	check_start_end_syntax(t_data *data)
{
	t_token	*current;

	current = data->tokens;
	if (current && current->type == TOKEN_OPERATOR
		&& ft_strcmp(current->value, "|") == 0)
	{
		ft_fprintf(2, "syntax error near unexpected token `%s'\n",
			current->value);
		data->last_exit_status = 2;
		return (-1);
	}
	while (current && current->next)
		current = current->next;
	if (current && current->type == TOKEN_OPERATOR)
	{
		ft_fprintf(2, "syntax error near unexpected token `newline'\n");
		data->last_exit_status = 2;
		return (1);
	}
	return (0);
}

static int	check_operator_continuity(t_data *data)
{
	t_token	*current;

	current = data->tokens;
	while (current)
	{
		if (current->type == TOKEN_OPERATOR)
		{
			if (ft_strcmp(current->value, "|") != 0 && current->next
				&& current->next->type == TOKEN_OPERATOR)
			{
				ft_fprintf(2, "syntax error near unexpected token `%s'\n",
					current->next->value);
				data->last_exit_status = 2;
				return (1);
			}
			if (ft_strcmp(current->value, "|") == 0 && !current->next)
			{
				ft_fprintf(2, "syntax error near unexpected token `|'\n");
				data->last_exit_status = 2;
				return (1);
			}
		}
		current = current->next;
	}
	return (0);
}

int	validate_syntax(t_data *data)
{
	if (check_start_end_syntax(data) != 0)
		return (-1);
	if (check_operator_continuity(data) != 0)
		return (1);
	data->last_exit_status = 0;
	return (0);
}
