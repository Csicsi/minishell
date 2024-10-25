#include "../includes/minishell.h"

int	handle_input_redirection(t_cmd *cmd, t_data *data)
{
	int	fd_in;

	if (cmd->input)
	{
		fd_in = open(cmd->input, O_RDONLY);
		if (fd_in < 0)
		{
			ft_fprintf(2, ": %s: No such file or directory\n", cmd->input);
			data->last_exit_status = 1;
			return (-1);
		}
		dup2(fd_in, STDIN_FILENO);
		close(fd_in);
	}
	else if (cmd->is_heredoc)
	{
		fd_in = handle_heredoc(cmd);
		dup2(fd_in, STDIN_FILENO);
		close(fd_in);
	}
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

int	handle_command_not_found(t_cmd *cmd, t_data *data)
{
	if (ft_strchr(cmd->args[0], '/'))
		ft_fprintf(2, ": %s: No such file or directory\n", cmd->args[0]);
	else
		ft_fprintf(2, ": %s: command not found\n", cmd->args[0]);
	data->last_exit_status = 127;
	return (data->last_exit_status);
}

int	setup_command_execution(t_cmd *cmd, t_data *data, char **cmd_path)
{
	if (handle_input_redirection(cmd, data) < 0)
		return (data->last_exit_status);
	if (handle_output_redirection(cmd, data) < 0)
		return (data->last_exit_status);
	if (is_builtin(cmd->args[0]))
	{
		data->last_exit_status = execute_builtin(cmd, data, false);
		return (data->last_exit_status);
	}
	*cmd_path = find_cmd_path(cmd->args, data);
	if (!*cmd_path)
		return (handle_command_not_found(cmd, data));
	if (update_last_command_env_var(data, *cmd_path) == -1)
	{
		free(*cmd_path);
		data->last_exit_status = 1;
		return (1);
	}
	return (0);
}

int	execute_single_cmd(t_cmd *cmd, t_data *data)
{
	char	*cmd_path;

	cmd_path = NULL;
	if (setup_command_execution(cmd, data, &cmd_path) != 0)
		return (data->last_exit_status);
	if (execve(cmd_path, cmd->args, data->env_vars) == -1)
	{
		if (ft_strchr(cmd->args[0], '/'))
			ft_fprintf(2, ": %s: Is a directory\n", cmd->args[0]);
		else
			ft_fprintf(2, ": %s: command not found\n", cmd->args[0]);
		if (ft_strchr(cmd->args[0], '/'))
			data->last_exit_status = 126;
		else
			data->last_exit_status = 127;
		free(cmd_path);
		return (data->last_exit_status);
	}
	free(cmd_path);
	return (0);
}
