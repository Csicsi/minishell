/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_handlers_bonus.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcsicsak <dcsicsak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/12 13:26:35 by dcsicsak          #+#    #+#             */
/*   Updated: 2024/11/15 16:25:34 by dcsicsak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell_bonus.h"

static t_cmd	*handle_pipe(t_cmd *current_cmd,
	int *arg_index, t_token *tokens)
{
	int	words_count;

	current_cmd->args[*arg_index] = NULL;
	current_cmd->next = initialize_cmd();
	if (!current_cmd->next)
		return (NULL);
	*arg_index = 0;
	words_count = count_tokens(tokens);
	current_cmd->next->args = ft_calloc(words_count + 1, sizeof(char *));
	if (current_cmd->next->args)
		return (current_cmd->next);
	else
		return (NULL);
}

static bool	handle_token_operator(t_data *data,
	t_cmd **current_cmd, t_parse_context *context)
{
	if (ft_strcmp(data->tokens->value, "<<") == 0)
		process_heredoc(*current_cmd, &data->tokens);
	else if (ft_strcmp(data->tokens->value, ">") == 0
		|| ft_strcmp(data->tokens->value, ">>") == 0)
		handle_output(*current_cmd, &data->tokens, &context->has_output);
	else if (ft_strcmp(data->tokens->value, "<") == 0)
		handle_input(*current_cmd, &data->tokens, &context->has_input);
	else if (ft_strcmp(data->tokens->value, "|") == 0)
	{
		(*current_cmd)->type = CMD_PIPE;
		*current_cmd = handle_pipe(*current_cmd,
				&context->arg_index, data->tokens);
		if (!*current_cmd)
			return (false);
		context->has_input = false;
		context->has_output = false;
	}
	else if (ft_strcmp(data->tokens->value, "&&") == 0)
	{
		(*current_cmd)->type = CMD_AND;
		*current_cmd = handle_pipe(*current_cmd, &context->arg_index,
				data->tokens);
		if (!*current_cmd)
			return (false);
		context->has_input = false;
		context->has_output = false;
	}
	else if (ft_strcmp(data->tokens->value, "||") == 0)
	{
		(*current_cmd)->type = CMD_OR;
		*current_cmd = handle_pipe(*current_cmd, &context->arg_index,
				data->tokens);
		if (!*current_cmd)
			return (false);
		context->has_input = false;
		context->has_output = false;
	}
	return (true);
}

bool	parse_single_token(t_data *data,
	t_cmd **current_cmd, t_parse_context *context)
{
	if (data->tokens->type == TOKEN_ERROR)
	{
		data->syntax_error = true;
		return (false);
	}
	else if (data->tokens->type == TOKEN_WORD)
	{
		(*current_cmd)->skip_execution = false;
		(*current_cmd)->args[context->arg_index++]
			= ft_strdup(data->tokens->value);
	}
	else if (data->tokens->type == TOKEN_EMPTY)
	{
		if (!(*current_cmd)->args[0])
			(*current_cmd)->skip_execution = true;
	}
	else if (data->tokens->type == TOKEN_OPERATOR)
	{
		return (handle_token_operator(data, current_cmd, context));
	}
	return (true);
}
