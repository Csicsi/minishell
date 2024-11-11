#include "../includes/minishell.h"

static int	handle_file_input_redirection(t_file *input_file, t_data *data)
{
	int	fd_in;

	fd_in = open(input_file->filename, O_RDONLY);
	if (fd_in < 0)
	{
		ft_fprintf(2, ": %s: No such file or directory\n",
			input_file->filename);
		data->last_exit_status = 1;
		return (-1);
	}
	dup2(fd_in, STDIN_FILENO);
	close(fd_in);
	return (0);
}

static int	handle_heredoc_input_redirection(t_cmd *cmd, t_data *data)
{
	int	fd_in;

	fd_in = open(cmd->heredoc_tempfile, O_RDONLY);
	if (fd_in < 0)
	{
		perror("open");
		data->last_exit_status = 1;
		return (-1);
	}
	dup2(fd_in, STDIN_FILENO);
	close(fd_in);
	free(cmd->heredoc_tempfile);
	cmd->heredoc_tempfile = NULL;
	return (0);
}

int	handle_input_redirection(t_cmd *cmd, t_data *data)
{
	t_file	*input_file;

	input_file = cmd->input_files;
	while (input_file)
	{
		if (check_input_error(input_file, cmd, data))
			return (-1);
		if (handle_file_input_redirection(input_file, data) < 0)
			return (-1);
		input_file = input_file->next;
	}
	if (cmd->is_heredoc && cmd->heredoc_tempfile)
		return (handle_heredoc_input_redirection(cmd, data));
	return (0);
}

static int	handle_file_output_redirection(t_file *output_file, t_data *data)
{
	int	fd_out;

	int dir_fd = open(output_file->filename, O_DIRECTORY);
	if (dir_fd >= 0)
	{
		close(dir_fd);
		write(2, output_file->filename, strlen(output_file->filename));
		write(2, ": Is a directory\n", 17);
		data->last_exit_status = 1;
		return (-1);
	}
	if (output_file->append)
		fd_out = open(output_file->filename,
				O_WRONLY | O_CREAT | O_APPEND, 0644);
	else
		fd_out = open(output_file->filename,
				O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd_out < 0)
	{
		ft_fprintf(2, ": %s: Permission denied\n", output_file->filename);
		data->last_exit_status = 1;
		return (-1);
	}
	dup2(fd_out, STDOUT_FILENO);
	close(fd_out);
	return (0);
}

int	handle_output_redirection(t_cmd *cmd, t_data *data)
{
	t_file	*output_file;

	output_file = cmd->output_files;
	while (output_file)
	{
		if (check_output_error(output_file, cmd, data))
			return (-1);
		if (handle_file_output_redirection(output_file, data) < 0)
			return (-1);
		output_file = output_file->next;
	}
	return (0);
}
