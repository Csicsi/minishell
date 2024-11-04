#include "../includes/minishell.h"

bool	check_input_error(t_cmd *cmd, t_data *data)
{
	if (cmd->input && access(cmd->input, F_OK) != 0)
	{
		ft_fprintf(2, ": %s: No such file or directory\n", cmd->input);
		cmd->io_error = true;
		data->last_exit_status = 1;
		return (true);
	}
	return (false);
}

bool	check_output_error(t_cmd *cmd, t_data *data)
{
	char	*output_dir;

	output_dir = get_directory_from_path(cmd->output);
	if (cmd->output && output_dir && access(output_dir, F_OK) != 0)
	{
		ft_fprintf(2, ": %s: No such file or directory\n", cmd->output);
		cmd->io_error = true;
		data->last_exit_status = 1;
		free(output_dir);
		return (true);
	}
	free(output_dir);
	return (false);
}

int	validate_cmd_list(t_data *data)
{
	t_cmd	*current;

	current = data->cmd_list;
	while (current != NULL)
	{
		current->io_error = false;
		check_input_error(current, data);
		check_output_error(current, data);
		current = current->next;
	}
	return (0);
}

