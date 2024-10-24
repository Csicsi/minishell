#include "../includes/minishell.h"

static void	ft_free(void **ptr)
{
	if (*ptr)
	{
		free(*ptr);
		*ptr = NULL;
	}
}

static void	free_cmd_list(t_cmd *cmd_list)
{
	t_cmd	*tmp;
	int			i;

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
		ft_free((void **)&cmd_list->input);
		ft_free((void **)&cmd_list->output);
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

void	cleanup_data(t_data *data, bool free_env)
{
	if (data->cmd_list)
	{
		free_cmd_list(data->cmd_list);
		data->cmd_list = NULL;
	}
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
