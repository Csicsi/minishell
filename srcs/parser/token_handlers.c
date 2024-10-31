#include "../../includes/minishell.h"

static bool	process_heredoc(t_cmd *cmd, t_token **tokens)
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

static bool	handle_output(t_cmd *cmd,
	t_token **tokens, bool *has_input, bool *has_output)
{
	int	fd_out;

	if ((*tokens)->next)
	{
		cmd->output = ft_strdup((*tokens)->next->value);
		cmd->append_output = (ft_strcmp((*tokens)->value, ">>") == 0);
		if (cmd->append_output)
			fd_out = open(cmd->output, O_WRONLY | O_CREAT | O_APPEND, 0644);
		else
			fd_out = open(cmd->output, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		close(fd_out);
		*tokens = (*tokens)->next;
		*has_output = true;
		if (*has_input)
			cmd->io_flag = IO_INPUT_FIRST;
		else
			cmd->io_flag = IO_OUTPUT_FIRST;
		return (true);
	}
	return (false);
}

static bool	handle_input(t_cmd *cmd,
	t_token **tokens, bool *has_input, bool *has_output)
{
	if (!cmd->input && (*tokens)->next)
	{
		cmd->input = ft_strdup((*tokens)->next->value);
		*tokens = (*tokens)->next;
		*has_input = true;
		if (*has_output)
			cmd->io_flag = IO_OUTPUT_FIRST;
		else
			cmd->io_flag = IO_INPUT_FIRST;
		return (true);
	}
	return (false);
}

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

bool	parse_single_token(t_data *data,
	t_cmd **current_cmd, t_parse_context *context)
{
	if (data->tokens->type == TOKEN_WORD)
		(*current_cmd)->args[context->arg_index++]
			= ft_strdup(data->tokens->value);
	else if (data->tokens->type == TOKEN_OPERATOR)
	{
		if (ft_strcmp(data->tokens->value, "<<") == 0)
			process_heredoc(*current_cmd, &data->tokens);
		else if (ft_strcmp(data->tokens->value, ">") == 0
			|| ft_strcmp(data->tokens->value, ">>") == 0)
			handle_output(*current_cmd, &data->tokens,
				&context->has_input, &context->has_output);
		else if (ft_strcmp(data->tokens->value, "<") == 0)
			handle_input(*current_cmd, &data->tokens,
				&context->has_input, &context->has_output);
		else if (ft_strcmp(data->tokens->value, "|") == 0)
		{
			*current_cmd = handle_pipe(*current_cmd,
					&context->arg_index, data->tokens);
			if (!*current_cmd)
				return (false);
			context->has_input = false;
			context->has_output = false;
		}
	}
	return (true);
}
