#include "../includes/minishell.h"

static void	check_input_error(t_cmd *cmd, t_data *data)
{
	if (cmd->input && access(cmd->input, F_OK) != 0)
	{
		ft_fprintf(2, ": %s: No such file or directory\n", cmd->input);
		cmd->io_error = true;
		data->last_exit_status = 1;
	}
}

static void	check_output_error(t_cmd *cmd, t_data *data)
{
	char	*output_dir;

	output_dir = get_directory_from_path(cmd->output);
	if (cmd->output && output_dir && access(output_dir, F_OK) != 0)
	{
		ft_fprintf(2, ": %s: No such file or directory\n", cmd->output);
		cmd->io_error = true;
		data->last_exit_status = 1;
	}
	free(output_dir);
}

static void	validate_input_output_first(t_cmd *cmd, t_data *data)
{
	if (cmd->io_flag == IO_INPUT_FIRST)
	{
		check_input_error(cmd, data);
		if (!cmd->io_error)
			check_output_error(cmd, data);
	}
	else if (cmd->io_flag == IO_OUTPUT_FIRST)
	{
		check_output_error(cmd, data);
		if (!cmd->io_error)
			check_input_error(cmd, data);
	}
}

static void	validate_io_neither_first(t_cmd *cmd, t_data *data)
{
	check_input_error(cmd, data);
	check_output_error(cmd, data);
}

int	validate_cmd_list(t_data *data)
{
	t_cmd	*current;

	current = data->cmd_list;
	while (current != NULL)
	{
		current->io_error = false;
		if (current->io_flag == IO_INPUT_FIRST
			|| current->io_flag == IO_OUTPUT_FIRST)
			validate_input_output_first(current, data);
		else
			validate_io_neither_first(current, data);
		current = current->next;
	}
	return (0);
}
