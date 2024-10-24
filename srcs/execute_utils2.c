/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_utils2.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcsicsak <dcsicsak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/20 13:28:03 by krabitsc          #+#    #+#             */
/*   Updated: 2024/10/24 11:13:50 by dcsicsak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

static int	print_syntax_error(t_token *cur, t_data *data);
static int	check_pipe_syntax(t_token *cur, t_data *data);
static int	check_redirection_syntax(t_token *cur, t_data *data);
static int	check_optype(const char *cur_value);

int	validate_token_syntax(t_token *tokens, t_data *data)
{
	t_token	*cur;

	cur = tokens;
	if (!cur || (cur->type != TOKEN_WORD && cur->type != TOKEN_OPERATOR))
		return (-1);
	while (cur)
	{
		if (cur->type == TOKEN_OPERATOR && ft_strcmp(cur->value, "|") == 0)
		{
			// Check for syntax errors related to pipes
			if (check_pipe_syntax(cur, data) == -1)
				return (-1);
		}
		else if (cur->type == TOKEN_OPERATOR && check_optype(cur->value))
		{
			// Check for syntax errors related to redirection
			if (check_redirection_syntax(cur, data) == -1)
				return (-1);
		}
		cur = cur->next;
	}
	return (0);
}

static int	print_syntax_error(t_token *cur, t_data *data)
{
	if (cur)
		ft_fprintf(2, ": syntax error near unexpected token `%s'\n", cur->value);
	else
		ft_fprintf(2, ": syntax error near unexpected token `newline'\n");
	data->last_exit_status = 2;
	return (-1);
}

static int	check_pipe_syntax(t_token *cur, t_data *data)
{
	// Move to next token after '|'
	cur = cur->next;
	// If there is no token or the token is not a word or redirection, print error
	if (!cur || (cur->type != TOKEN_WORD && (ft_strcmp(cur->value, ">") != 0 && ft_strcmp(cur->value, "<") != 0)))
		return (print_syntax_error(cur, data));
	return (0);
}

static int	check_redirection_syntax(t_token *cur, t_data *data)
{
	// Move to next token after '>' or '<'
	cur = cur->next;
	// If there is no token or the token is not a word, print error
	if (!cur || cur->type != TOKEN_WORD)
		return (print_syntax_error(cur, data));
	return (0);
}

static int	check_optype(const char *cur_value)
{
	if (ft_strcmp(cur_value, ">") == 0
		|| ft_strcmp(cur_value, ">>") == 0
		|| ft_strcmp(cur_value, "<") == 0
		|| ft_strcmp(cur_value, "<<") == 0)
		return (1);
	return (0);
}


int	count_cmds(t_cmd *cmd_list)
{
	int	count;

	count = 0;
	while (cmd_list)
	{
		count++;
		cmd_list = cmd_list->next;
	}
	return (count);
}

