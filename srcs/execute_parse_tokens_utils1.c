/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_parse_tokens_utils1.c                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krabitsc <krabitsc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/21 07:31:36 by krabitsc          #+#    #+#             */
/*   Updated: 2024/10/21 09:43:13 by krabitsc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

static t_cmd	*initialize_cmd(t_data *data);
static t_cmd	*case_pipe(t_cmd *cur_cmd, int *arg_index, t_data *data);
static int		parse_tok_loop(t_cmd **cur_cmd, int *arg_index, t_data *data);

t_cmd	*parse_tokens(t_data *data)
{
	t_cmd	*cmd;
	t_cmd	*cur_cmd;
	int		arg_index;
	t_token	*tmp;

	cmd = initialize_cmd(data);
	if (!cmd)
		return (NULL);
	cur_cmd = cmd;
	tmp = data->tokens;
	arg_index = 0;
	if (parse_tok_loop(&cur_cmd, &arg_index, data) == -1)
		return (NULL);
	cur_cmd->args[arg_index] = NULL;
	data->tokens = tmp;
	free_tokens(data);
	return (cmd);
}

static t_cmd	*initialize_cmd(t_data *data)
{
	int		words_count;
	t_cmd	*cur_cmd;

	words_count = count_words(data->tokens);
	cur_cmd = malloc(sizeof(t_cmd));
	if (!cur_cmd)
		return (NULL);
	cur_cmd->args = ft_calloc(words_count + 1, sizeof(char *));
	if (!cur_cmd->args)
		return (NULL);
	cur_cmd->input = NULL;
	cur_cmd->output = NULL;
	cur_cmd->append_output = 0;
	cur_cmd->exit_status = 0;
	cur_cmd->heredoc_delim = NULL;
	cur_cmd->is_heredoc = false;
	cur_cmd->next = NULL;
	return (cur_cmd);
}

static int	parse_tok_loop(t_cmd **cur_cmd, int *arg_index, t_data *data)
{
	while (data->tokens)
	{
		if (data->tokens->type == TOKEN_WORD)
			(*cur_cmd)->args[(*arg_index)++] = ft_strdup(data->tokens->value);
		else if (data->tokens->type == TOKEN_OPERATOR
			&& ft_strcmp(data->tokens->value, "|") == 0)
		{
			*cur_cmd = case_pipe(*cur_cmd, arg_index, data);
			if (!*cur_cmd)
				return (-1);
		}
		else if (data->tokens->type == TOKEN_OPERATOR)
		{
			if (case_redirection(*cur_cmd, arg_index, data) == -1)
				return (-1);
		}
		if (data->tokens)
			data->tokens = data->tokens->next;
	}
	return (0);
}

static t_cmd	*case_pipe(t_cmd *cur_cmd, int *arg_index, t_data *data)
{
	cur_cmd->args[*arg_index] = NULL;
	cur_cmd->next = initialize_cmd(data);
	if (!cur_cmd->next)
		return (NULL);
	*arg_index = 0;
	return (cur_cmd->next);
}
