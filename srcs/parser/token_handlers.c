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

static bool	handle_output(t_cmd *cmd, t_token **tokens,
	bool *has_output)
{
	t_file	*new_output;
	t_file	*last;

	if ((*tokens)->next)
	{
		if ((*tokens)->next->type == TOKEN_EMPTY)
		{
			ft_fprintf(2, "%s: ambiguous redirect\n", (*tokens)->next->old_value);
			cmd->empty_redir = true;
			return (false);
		}
		new_output = malloc(sizeof(t_file));
		if (!new_output)
			return (false);
		new_output->filename = ft_strdup((*tokens)->next->value);
		new_output->append = (ft_strcmp((*tokens)->value, ">>") == 0);
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
		*tokens = (*tokens)->next;
		*has_output = true;
		if (cmd->redirection_order == NO_REDIRECTION)
			cmd->redirection_order = OUTPUT_FIRST;
		return (true);
	}
	return (false);
}

static bool	handle_input(t_cmd *cmd, t_token **tokens,
	bool *has_input)
{
	t_file	*new_input;
	t_file	*last;

	if ((*tokens)->next)
	{
		if ((*tokens)->next->type == TOKEN_EMPTY)
		{
			ft_fprintf(2, "%s: ambiguous redirect\n", (*tokens)->next->old_value);
			cmd->empty_redir = true;
			return (false);
		}
		new_input = malloc(sizeof(t_file));
		if (!new_input)
			return (false);
		new_input->filename = ft_strdup((*tokens)->next->value);
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
		*tokens = (*tokens)->next;
		*has_input = true;
		if (cmd->redirection_order == NO_REDIRECTION)
			cmd->redirection_order = INPUT_FIRST;
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

bool parse_single_token(t_data *data,
	t_cmd **current_cmd, t_parse_context *context)
{
	if (data->tokens->type == TOKEN_ERROR)
	{
		data->syntax_error = true;
		return (false);
	}
	if (data->tokens->type == TOKEN_WORD)
	{
		(*current_cmd)->skip_execution = false;
		(*current_cmd)->args[context->arg_index++]
			= ft_strdup(data->tokens->value);
	}
	else if (data->tokens->type == TOKEN_EMPTY && !(*current_cmd)->args[0])
		(*current_cmd)->skip_execution = true;
	else if (data->tokens->type == TOKEN_OPERATOR)
	{
		if (ft_strcmp(data->tokens->value, "<<") == 0)
			process_heredoc(*current_cmd, &data->tokens);
		else if (ft_strcmp(data->tokens->value, ">") == 0
			|| ft_strcmp(data->tokens->value, ">>") == 0)
			handle_output(*current_cmd, &data->tokens,
				&context->has_output);
		else if (ft_strcmp(data->tokens->value, "<") == 0)
			handle_input(*current_cmd, &data->tokens,
				&context->has_input);
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
