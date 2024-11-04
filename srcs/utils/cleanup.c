#include "../includes/minishell.h"

static void	free_file_list(t_file *file_list)
{
	t_file	*temp;

	while (file_list)
	{
		temp = file_list;
		file_list = file_list->next;
		ft_free((void **)&temp->filename);
		ft_free((void **)&temp);
	}
}

static void	free_cmd_list(t_cmd *cmd_list)
{
	t_cmd	*tmp;
	int		i;

	while (cmd_list)
	{
		tmp = cmd_list;
		if (cmd_list->args)
		{
			i = 0;
			while (cmd_list->args[i])
			{
				ft_free((void **)&cmd_list->args[i]);
				i++;
			}
			ft_free((void **)&cmd_list->args);
		}
		free_file_list(cmd_list->input_files);
		free_file_list(cmd_list->output_files);
		ft_free((void **)&cmd_list->heredoc_delim);
		cmd_list = cmd_list->next;
		ft_free((void **)&tmp);
	}
}

void	free_tokens(t_data *data)
{
	t_token	*temp;

	while (data->tokens)
	{
		temp = data->tokens;
		data->tokens = data->tokens->next;
		if (temp->value)
		{
			free(temp->value);
			temp->value = NULL;
		}
		free(temp);
		temp = NULL;
	}
	data->tokens = NULL;
}

void	free_string_array(char **string_array)
{
	int	i;

	if (!string_array)
		return ;
	i = 0;
	while (string_array[i])
	{
		free(string_array[i]);
		string_array[i] = NULL;
		i++;
	}
	free(string_array);
	string_array = NULL;
}

static void	cleanup_cmd_list(t_data *data)
{
	t_cmd	*current_cmd;

	if (data->cmd_list)
	{
		current_cmd = data->cmd_list;
		while (current_cmd)
		{
			if (current_cmd->heredoc_tempfile)
			{
				if (access(current_cmd->heredoc_tempfile, F_OK) == 0)
				{
					if (unlink(current_cmd->heredoc_tempfile) != 0)
						perror("unlink");
				}
				free(current_cmd->heredoc_tempfile);
				current_cmd->heredoc_tempfile = NULL;
			}
			current_cmd = current_cmd->next;
		}
		free_cmd_list(data->cmd_list);
		data->cmd_list = NULL;
	}
}

void	cleanup_data(t_data *data, bool free_env)
{
	cleanup_cmd_list(data);
	if (data->tokens)
	{
		free_tokens(data);
		data->tokens = NULL;
	}
	if (data->env_vars && free_env)
	{
		free_string_array(data->env_vars);
		data->env_vars = NULL;
	}
	if (data->input)
	{
		free(data->input);
		data->input = NULL;
	}
}
