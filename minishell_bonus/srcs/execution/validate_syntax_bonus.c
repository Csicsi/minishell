/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   validate_syntax_bonus.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcsicsak <dcsicsak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/12 13:25:58 by dcsicsak          #+#    #+#             */
/*   Updated: 2024/11/15 15:37:07 by dcsicsak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell_bonus.h"

static int	print_syn_err(t_data *data,
	int error_flag, const char *token, t_token *curr)
{
	curr->type = TOKEN_ERROR;
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
	t_token	*curr;
	t_token	*first;
	int		brackets;

	brackets = 0;
	first = data->tokens;
	curr = data->tokens;
	if (curr && curr->type == 1 && ft_strcmp(curr->value, "|") == 0)
		return (print_syn_err(data, 3, curr->value, curr));
	while (curr)
	{
		if (curr->type == TOKEN_OPERATOR)
		{
			if (is_redir(curr) && !curr->next)
				return (print_syn_err(data, 2, NULL, curr));
			if (ft_strcmp(curr->value, "|") != 0 && curr->next
				&& curr->next->type == TOKEN_OPERATOR)
				return (print_syn_err(data, 1, curr->next->value, curr));
			if ((ft_strcmp(curr->value, "|") == 0
					|| ft_strcmp(curr->value, "&&") == 0
					|| ft_strcmp(curr->value, "||") == 0) && !curr->next)
				return (print_syn_err(data, 1, curr->value, first));
		}
		if (curr->type == TOKEN_OPEN)
		{
			if (curr->next && curr->next->type == TOKEN_CLOSE)
				return (print_syn_err(data, 1, ")", curr));
			brackets++;
		}
		if (curr->type == TOKEN_CLOSE)
		{
			if (curr->next && curr->next->type == TOKEN_OPEN)
				return (print_syn_err(data, 1, "(", curr));
			brackets--;
		}
		curr = curr->next;
	}
	if (brackets > 0)
		return (print_syn_err(data, 1, "newline", first));
	else if (brackets < 0)
		return (print_syn_err(data, 1, ")", first));
	data->last_exit_status = 0;
	return (0);
}
