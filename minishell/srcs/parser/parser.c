/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcsicsak <dcsicsak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/12 13:26:27 by dcsicsak          #+#    #+#             */
/*   Updated: 2024/11/18 11:23:52 by dcsicsak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

t_cmd	*initialize_cmd(void)
{
	t_cmd	*new_cmd;

	new_cmd = ft_calloc(1, sizeof(t_cmd));
	if (!new_cmd)
		return (NULL);
	new_cmd->args = NULL;
	new_cmd->input_files = NULL;
	new_cmd->output_files = NULL;
	new_cmd->append_output = false;
	new_cmd->exit_status = 0;
	new_cmd->heredoc_delim = NULL;
	new_cmd->heredoc_tempfile = NULL;
	new_cmd->heredoc_count = 0;
	new_cmd->is_heredoc = false;
	new_cmd->next = NULL;
	new_cmd->skip_execution = false;
	new_cmd->io_error = false;
	new_cmd->empty_redir = false;
	new_cmd->redirection_order = NO_REDIRECTION;
	return (new_cmd);
}

int	count_tokens(t_token *tokens)
{
	int	count;

	count = 0;
	while (tokens)
	{
		count++;
		tokens = tokens->next;
	}
	return (count);
}

bool	finalize_heredoc(t_cmd *current_cmd)
{
	char	**new_delim_array;

	if (current_cmd->heredoc_delim)
	{
		new_delim_array = ft_realloc(current_cmd->heredoc_delim,
				sizeof(char *) * current_cmd->heredoc_count,
				sizeof(char *) * (current_cmd->heredoc_count + 1));
		if (!new_delim_array)
			return (false);
		current_cmd->heredoc_delim = new_delim_array;
		current_cmd->heredoc_delim[current_cmd->heredoc_count] = NULL;
	}
	return (true);
}

bool	parse_tokens_loop(t_data *data, t_cmd **current_cmd,
	t_parse_context *context, t_token *tmp)
{
	while (data->tokens)
	{
		if (!parse_single_token(data, current_cmd, context))
		{
			data->tokens = tmp;
			return (false);
		}
		data->tokens = data->tokens->next;
	}
	return (true);
}

t_cmd	*parse_tokens(t_data *data)
{
	t_cmd			*cmd;
	t_cmd			*current_cmd;
	t_parse_context	context;
	t_token			*tmp;
	int				words_count;

	cmd = initialize_cmd();
	if (!cmd)
		return (NULL);
	current_cmd = cmd;
	context = (t_parse_context){0, false, false};
	tmp = data->tokens;
	words_count = count_tokens(data->tokens);
	current_cmd->args = ft_calloc(words_count + 1, sizeof(char *));
	if (!current_cmd->args)
		return (NULL);
	if (!parse_tokens_loop(data, &current_cmd, &context, tmp))
		return (cmd);
	current_cmd->args[context.arg_index] = NULL;
	data->tokens = tmp;
	if (!finalize_heredoc(current_cmd))
		return (NULL);
	return (cmd);
}
