#include "../includes/minishell.h"

t_cmd	*initialize_cmd(void)
{
	t_cmd	*new_cmd;

	new_cmd = ft_calloc(1, sizeof(t_cmd));
	if (!new_cmd)
		return (NULL);
	new_cmd->args = NULL;
	new_cmd->input = NULL;
	new_cmd->output = NULL;
	new_cmd->append_output = false;
	new_cmd->exit_status = 0;
	new_cmd->heredoc_delim = NULL;
	new_cmd->is_heredoc = false;
	new_cmd->io_flag = IO_NONE_OR_SINGLE;
	new_cmd->next = NULL;
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

t_cmd	*parse_tokens(t_data *data)
{
	int				words_count;
	t_cmd			*cmd;
	t_cmd			*current_cmd;
	t_parse_context	context;
	t_token			*tmp;

	cmd = initialize_cmd();
	if (!cmd)
		return (NULL);
	current_cmd = cmd;
	context = (t_parse_context){0, false, false};
	words_count = count_tokens(data->tokens);
	current_cmd->args = ft_calloc(words_count + 1, sizeof(char *));
	if (!current_cmd->args)
		return (NULL);
	tmp = data->tokens;
	while (data->tokens)
	{
		if (!parse_single_token(data, &current_cmd, &context))
			return (NULL);
		data->tokens = data->tokens->next;
	}
	current_cmd->args[context.arg_index] = NULL;
	data->tokens = tmp;
	return (cmd);
}
