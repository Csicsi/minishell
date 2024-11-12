/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_io.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcsicsak <dcsicsak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/12 13:39:25 by dcsicsak          #+#    #+#             */
/*   Updated: 2024/11/12 13:39:48 by dcsicsak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

bool	process_heredoc(t_cmd *cmd, t_token **tokens)
{
	if (!cmd->is_heredoc && (*tokens)->next)
	{
		cmd->is_heredoc = true;
		cmd->heredoc_delim = ft_strdup((*tokens)->next->value);
		*tokens = (*tokens)->next;
		return (true);
	}
	return (false);
}

static bool	add_output_file(t_cmd *cmd, t_token *token, bool append)
{
	t_file	*new_output;
	t_file	*last;

	new_output = malloc(sizeof(t_file));
	if (!new_output)
		return (false);
	new_output->filename = ft_strdup(token->value);
	new_output->append = append;
	new_output->next = NULL;
	if (cmd->output_files == NULL)
		cmd->output_files = new_output;
	else
	{
		last = cmd->output_files;
		while (last->next)
			last = last->next;
		last->next = new_output;
	}
	return (true);
}

bool	handle_output(t_cmd *cmd, t_token **tokens, bool *has_output)
{
	bool	append;

	if ((*tokens)->next)
	{
		if ((*tokens)->next->type == TOKEN_EMPTY)
		{
			ft_fprintf(2, "%s: ambiguous redirect\n",
				(*tokens)->next->old_value);
			cmd->empty_redir = true;
			return (false);
		}
		append = (ft_strcmp((*tokens)->value, ">>") == 0);
		if (!add_output_file(cmd, (*tokens)->next, append))
			return (false);
		*tokens = (*tokens)->next;
		*has_output = true;
		if (cmd->redirection_order == NO_REDIRECTION)
			cmd->redirection_order = OUTPUT_FIRST;
		return (true);
	}
	return (false);
}

static bool	add_input_file(t_cmd *cmd, t_token *token)
{
	t_file	*new_input;
	t_file	*last;

	new_input = malloc(sizeof(t_file));
	if (!new_input)
		return (false);
	new_input->filename = ft_strdup(token->value);
	new_input->append = false;
	new_input->next = NULL;
	if (cmd->input_files == NULL)
		cmd->input_files = new_input;
	else
	{
		last = cmd->input_files;
		while (last->next)
			last = last->next;
		last->next = new_input;
	}
	return (true);
}

bool	handle_input(t_cmd *cmd, t_token **tokens, bool *has_input)
{
	if ((*tokens)->next)
	{
		if ((*tokens)->next->type == TOKEN_EMPTY)
		{
			ft_fprintf(2, "%s: ambiguous redirect\n",
				(*tokens)->next->old_value);
			cmd->empty_redir = true;
			return (false);
		}
		if (!add_input_file(cmd, (*tokens)->next))
			return (false);
		*tokens = (*tokens)->next;
		*has_input = true;
		if (cmd->redirection_order == NO_REDIRECTION)
			cmd->redirection_order = INPUT_FIRST;
		return (true);
	}
	return (false);
}
