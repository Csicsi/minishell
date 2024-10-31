#include "../includes/minishell.h"

int	redirect_output(t_cmd *current, t_data *data, pid_t *child_pids)
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
		free(child_pids);
		exit(1);
	}
	dup2(fd_out, STDOUT_FILENO);
	close(fd_out);
	return (0);
}

void	execute_child_command(t_cmd *current, t_data *data,
	int *prev_fd, int *pipe_fd, pid_t *child_pids)
{
	if (*prev_fd != -1)
	{
		dup2(*prev_fd, STDIN_FILENO);
		close(*prev_fd);
	}
	if (current->next != NULL)
	{
		dup2(pipe_fd[1], STDOUT_FILENO);
		close(pipe_fd[0]);
		close(pipe_fd[1]);
	}
	if (current->output != NULL)
		redirect_output(current, data, child_pids);
	if (is_builtin(current->args[0]))
		data->last_exit_status = execute_builtin(current, data, false);
	else
		data->last_exit_status = execute_single_cmd(current, data);
	cleanup_data(data, true);
	free(child_pids);
	exit(data->last_exit_status);
}

int	execute_command_in_list(t_cmd *current, t_data *data,
	int *prev_fd, int *pipe_fd, pid_t *child_pids, int num_children)
{
	pid_t	pid;

	if (current->next != NULL)
		pipe(pipe_fd);
	pid = fork();
	if (pid == 0)
		execute_child_command(current, data, prev_fd, pipe_fd, child_pids);
	else if (pid > 0)
	{
		child_pids[num_children] = pid;
		if (*prev_fd != -1)
			close(*prev_fd);
		if (current->next != NULL)
		{
			close(pipe_fd[1]);
			*prev_fd = pipe_fd[0];
		}
		else
			*prev_fd = -1;
	}
	else
		return (perror(": fork"), 1);
	return (0);
}

int	execute_single_builtin_if_needed(t_cmd *current,
	t_data *data, pid_t *child_pids)
{
	if (current->args[0] && ft_strcmp(current->args[0], "exit") == 0
		&& current->next == NULL)
	{
		data->last_exit_status = execute_builtin(current, data, true);
		cleanup_data(data, true);
		free(child_pids);
		return (data->last_exit_status);
	}
	if (current->args[0] != NULL && ((ft_strcmp(current->args[0], "export") == 0
				|| ft_strcmp(current->args[0], "unset") == 0
				|| ft_strcmp(current->args[0], "cd") == 0)
			&& current->next == NULL))
	{
		data->last_exit_status = execute_builtin(current, data, false);
		cleanup_data(data, false);
		free(child_pids);
		return (data->last_exit_status);
	}
	return (-1);
}

int	execute_all_commands_in_list(t_cmd *current, t_data *data, int *prev_fd,
	int *pipe_fd, pid_t *child_pids, int *num_children, int *io_error_status)
{
	while (current != NULL)
	{
		if (current->io_error)
		{
			*io_error_status = 1;
			current = current->next;
			continue ;
		}
		else
		{
			*io_error_status = 0;
		}
		if (execute_command_in_list(current,
				data, prev_fd, pipe_fd, child_pids, *num_children) == 1)
		{
			return (1);
		}
		(*num_children)++;
		current = current->next;
	}
	return (0);
}

int	execute_cmd_list(t_data *data)
{
	t_cmd	*current;
	int		pipe_fd[2];
	int		prev_fd;
	int		num_children;
	int		num_commands;
	pid_t	*child_pids;
	int		io_error_status;
	int		builtin_status;

	prev_fd = -1;
	num_children = 0;
	num_commands = count_cmds(data->cmd_list);
	io_error_status = 0;
	handle_heredoc(data->cmd_list, data);
	child_pids = malloc(sizeof(pid_t) * num_commands);
	if (!child_pids)
	{
		perror(": malloc");
		return (1);
	}
	current = data->cmd_list;
	builtin_status = execute_single_builtin_if_needed(current,
			data, child_pids);
	if (builtin_status != -1)
		return (builtin_status);
	if (execute_all_commands_in_list(current, data, &prev_fd,
			pipe_fd, child_pids, &num_children, &io_error_status) == 1)
	{
		cleanup_data(data, false);
		free(child_pids);
		return (1);
	}
	for (int i = 0; i < num_children; i++)
	{
		waitpid(child_pids[i], &data->last_exit_status, 0);
		if (WIFEXITED(data->last_exit_status))
			data->last_exit_status = WEXITSTATUS(data->last_exit_status);
	}
	cleanup_data(data, false);
	free(child_pids);
	if (io_error_status && data->last_exit_status == 0)
		data->last_exit_status = 1;
	return (data->last_exit_status);
}
