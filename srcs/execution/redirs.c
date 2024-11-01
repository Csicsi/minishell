#include "../includes/minishell.h"

static int	handle_file_input_redirection(t_cmd *cmd, t_data *data)
{
	int	fd_in;

	fd_in = open(cmd->input, O_RDONLY);
	if (fd_in < 0)
	{
		ft_fprintf(2, ": %s: No such file or directory\n", cmd->input);
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
	unlink(cmd->heredoc_tempfile);
	free(cmd->heredoc_tempfile);
	cmd->heredoc_tempfile = NULL;
	return (0);
}

int	handle_input_redirection(t_cmd *cmd, t_data *data)
{
	if (cmd->input)
		return (handle_file_input_redirection(cmd, data));
	else if (cmd->is_heredoc && cmd->heredoc_tempfile)
		return (handle_heredoc_input_redirection(cmd, data));
	return (0);
}

int	handle_output_redirection(t_cmd *cmd, t_data *data)
{
	int	fd_out;

	if (cmd->output)
	{
		if (cmd->append_output)
			fd_out = open(cmd->output, O_WRONLY | O_CREAT | O_APPEND, 0644);
		else
			fd_out = open(cmd->output, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if (fd_out < 0)
		{
			ft_fprintf(2, ": %s: Cannot open file\n", cmd->output);
			data->last_exit_status = 1;
			return (-1);
		}
		dup2(fd_out, STDOUT_FILENO);
		close(fd_out);
	}
	return (0);
}

int	redirect_output(t_cmd *current, t_data *data, t_exec_context *ctx)
{
	int	fd_out;

	if (current->append_output)
		fd_out = open(current->output, O_WRONLY | O_CREAT | O_APPEND, 0644);
	else
		fd_out = open(current->output, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd_out < 0)
	{
		if (ft_strchr(current->output, '/') != NULL)
			ft_fprintf(2, "%s: Is a directory\n", current->output);
		else
			ft_fprintf(2, "%s: Permission denied\n", current->output);
		cleanup_data(data, true);
		free(ctx->child_pids);
		exit(1);
	}
	dup2(fd_out, STDOUT_FILENO);
	close(fd_out);
	return (0);
}
