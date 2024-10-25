/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_utils2.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: csicsi <csicsi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/20 13:28:03 by krabitsc          #+#    #+#             */
/*   Updated: 2024/10/25 15:10:37 by csicsi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

static int	check_syntax_error(t_token *cur, t_data *data, int check_type)
{
	cur = cur->next;
	if (check_type == 1
		&& (!cur || (cur->type != TOKEN_WORD && ft_strcmp(cur->value, ">") != 0
				&& ft_strcmp(cur->value, "<") != 0)))
	{
		if (cur)
			ft_fprintf(2, ": syntax error near unexpected token `%s'\n",
				cur->value);
		else
			ft_fprintf(2, ": syntax error near unexpected token `newline'\n");
		data->last_exit_status = 2;
		return (-1);
	}
	else if (check_type == 2 && (!cur || cur->type != TOKEN_WORD))
	{
		if (cur)
			ft_fprintf(2, ": syntax error near unexpected token `%s'\n",
				cur->value);
		else
			ft_fprintf(2, ": syntax error near unexpected token `newline'\n");
		data->last_exit_status = 2;
		return (-1);
	}
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
			if (check_syntax_error(cur, data, 1) == -1)
				return (-1);
		}
		else if (cur->type == TOKEN_OPERATOR && check_optype(cur->value))
		{
			if (check_syntax_error(cur, data, 2) == -1)
				return (-1);
		}
		cur = cur->next;
	}
	return (0);
}
