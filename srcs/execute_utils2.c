/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_utils2.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krabitsc <krabitsc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/20 13:28:03 by krabitsc          #+#    #+#             */
/*   Updated: 2024/10/21 09:21:16 by krabitsc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

static int	print_syntax_error(t_token *cur, t_data *data);
static int	check_optype(const char *cur_value);

int	check_commands_in_tokens(t_token *tokens, t_data *data)
{
	t_token	*cur;

	cur = tokens;
	if (!cur || (cur->type != TOKEN_WORD && cur->type != TOKEN_OPERATOR))
		return (-1);
	while (cur)
	{
		if (cur->type == TOKEN_OPERATOR && ft_strcmp(cur->value, "|") == 0)
		{
			cur = cur->next;
			if (!cur || (cur->type != TOKEN_WORD
					&& (ft_strcmp(cur->value, ">") != 0
						&& ft_strcmp(cur->value, "<") != 0)))
				return (print_syntax_error(cur, data));
		}
		else if (cur->type == TOKEN_OPERATOR && (check_optype(cur->value)))
		{
			cur = cur->next;
			if (!cur || cur->type != TOKEN_WORD)
				return (print_syntax_error(cur, data));
		}
		cur = cur->next;
	}
	return (0);
}

static int	print_syntax_error(t_token *cur, t_data *data)
{
	if (cur)
		ft_fprintf(2, ": syntax error near unexpected token `%s'\n",
			cur->value);
	else
		ft_fprintf(2, ": syntax error near unexpected token `newline'\n");
	data->last_exit_status = 2;
	return (-1);
}

static int	check_optype(const char *cur_value)
{
	if (ft_strcmp(cur_value, ">") == 0
		|| ft_strcmp(cur_value, ">>") == 0
		|| ft_strcmp(cur_value, "<") == 0)
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

int	count_words(t_token *tokens)
{
	int	word_count;

	word_count = 0;
	while (tokens)
	{
		if ((ft_strcmp(tokens->value, "|") == 0) && tokens->next != NULL)
		{
			tokens = tokens->next;
		}
		if (tokens->type == TOKEN_WORD)
		{
			word_count++;
		}
		else if (tokens->type == TOKEN_OPERATOR)
		{
			if (ft_strcmp(tokens->value, "|") == 0)
				break ;
		}
		tokens = tokens->next;
	}
	return (word_count);
}
