/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_utils2.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krabitsc <krabitsc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/20 13:28:03 by krabitsc          #+#    #+#             */
/*   Updated: 2024/10/20 13:28:43 by krabitsc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"


int	check_commands_in_tokens(t_token *tokens, t_data *data)
{
	t_token	*current;

	current = tokens;
	if (!current || (current->type != TOKEN_WORD && current->type != TOKEN_OPERATOR))
		return (-1);
	while (current)
	{
		if (current->type == TOKEN_OPERATOR && ft_strcmp(current->value, "|") == 0)
		{
			current = current->next;
			if (!current || (current->type != TOKEN_WORD && (ft_strcmp(current->value, ">") != 0 && ft_strcmp(current->value, "<") != 0)))
			{
				if (current)
				{
					ft_fprintf(2, ": syntax error near unexpected token `%s'\n", current->value);
					data->last_exit_status = 2;
				}
				else
				{
					ft_fprintf(2, ": syntax error near unexpected token `newline'\n");
					data->last_exit_status = 2;
				}
				return (-1);
			}
		}
		else if (current->type == TOKEN_OPERATOR && (ft_strcmp(current->value, ">") == 0 || ft_strcmp(current->value, ">>") == 0 || ft_strcmp(current->value, "<") == 0))
		{
			current = current->next;
			if (!current || current->type != TOKEN_WORD)
			{
				if (current)
				{
					ft_fprintf(2, ": syntax error near unexpected token `%s'\n", current->value);
					data->last_exit_status = 2;
				}
				else
				{
					ft_fprintf(2, ": syntax error near unexpected token `newline'\n");
					data->last_exit_status = 2;
				}
				return (-1);
			}
		}
		current = current->next;
	}
	return (0);
}
