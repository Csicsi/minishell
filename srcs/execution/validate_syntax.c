#include "../includes/minishell.h"

static int	print_syntax_error(t_data *data, int error_flag, const char *token,  t_token *current)
{
	current->type = TOKEN_ERROR;
	if (error_flag == 1)
		ft_fprintf(2, "syntax error near unexpected token `%s'\n", token);
	else if (error_flag == 2)
		ft_fprintf(2, "syntax error near unexpected token `newline'\n");
	else if (error_flag == 3)
		ft_fprintf(2, "syntax error near unexpected token `|'\n");
	data->last_exit_status = 2;
	return (-1);
}

static int	is_redir(t_token *token)
{
	if (!token)
		return (0);
	return (ft_strcmp(token->value, ">") == 0
		|| ft_strcmp(token->value, ">>") == 0
		|| ft_strcmp(token->value, "<") == 0
		|| ft_strcmp(token->value, "<<") == 0);
}

int	validate_syntax(t_data *data)
{
	t_token	*current;

	current = data->tokens;
	if (current && current->type == TOKEN_OPERATOR
		&& ft_strcmp(current->value, "|") == 0)
		return (print_syntax_error(data, 3, current->value, current));
	while (current)
	{
		if (current->type == TOKEN_OPERATOR)
		{
			if (is_redir(current) && !current->next)
				return (print_syntax_error(data, 2, NULL, current));
			if (ft_strcmp(current->value, "|") != 0 && current->next
				&& current->next->type == TOKEN_OPERATOR)
				return (print_syntax_error(data, 1, current->next->value, current));
			if (ft_strcmp(current->value, "|") == 0 && !current->next)
				return (print_syntax_error(data, 3, NULL, current));
		}
		current = current->next;
	}
	if (current && current->type == TOKEN_OPERATOR
		&& ft_strcmp(current->value, "|") == 0)
		return (print_syntax_error(data, 3, current->value, current));
	data->last_exit_status = 0;
	return (0);
}
