#include "../includes/minishell.h"

static void	execute_child_command(t_cmd *current,
	t_data *data, t_exec_context *ctx)
{
	if (ctx->prev_fd != -1)
	{
		dup2(ctx->prev_fd, STDIN_FILENO);
		close(ctx->prev_fd);
	}
	if (current->next != NULL)
	{
		dup2(ctx->pipe_fd[1], STDOUT_FILENO);
		close(ctx->pipe_fd[0]);
		close(ctx->pipe_fd[1]);
	}
	if (current->redirection_order == OUTPUT_FIRST)
	{
		if (handle_output_redirection(current, data) < 0)
		{
			cleanup_data(data, true);
			free(ctx->child_pids);
			exit(data->last_exit_status);
		}
		if (handle_input_redirection(current, data) < 0)
		{
			cleanup_data(data, true);
			free(ctx->child_pids);
			exit(data->last_exit_status);
		}
	}
	else
	{
		if (handle_input_redirection(current, data) < 0)
		{
			cleanup_data(data, true);
			free(ctx->child_pids);
			exit(data->last_exit_status);
		}
		if (handle_output_redirection(current, data) < 0)
		{
			cleanup_data(data, true);
			free(ctx->child_pids);
			exit(data->last_exit_status);
		}
	}
	if (is_builtin(current->args[0]))
		data->last_exit_status = execute_builtin(current, data, false);
	else
		data->last_exit_status = execute_single_cmd(current, data);
	cleanup_data(data, true);
	free(ctx->child_pids);
	exit(data->last_exit_status);
}

static int	execute_command_in_list(t_cmd *current, t_data *data,
	t_exec_context *ctx)
{
	pid_t	pid;

	if (current->next != NULL)
		pipe(ctx->pipe_fd);
	pid = fork();
	if (pid == 0)
		execute_child_command(current, data, ctx);
	else if (pid > 0)
	{
		ctx->child_pids[ctx->num_children++] = pid;
		if (ctx->prev_fd != -1)
			close(ctx->prev_fd);
		if (current->next != NULL)
		{
			close(ctx->pipe_fd[1]);
			ctx->prev_fd = ctx->pipe_fd[0];
		}
		else
			ctx->prev_fd = -1;
	}
	else
		return (perror(": fork"), 1);
	return (0);
}

static int	execute_single_builtin_if_needed(t_cmd *current,
	t_data *data, t_exec_context *ctx)
{
	if (current->args[0] && ft_strcmp(current->args[0], "exit") == 0
		&& current->next == NULL)
	{
		data->last_exit_status = execute_builtin(current, data, true);
		cleanup_data(data, true);
		free(ctx->child_pids);
		return (data->last_exit_status);
	}
	if (current->args[0] != NULL && ((ft_strcmp(current->args[0], "export") == 0
				|| ft_strcmp(current->args[0], "unset") == 0
				|| ft_strcmp(current->args[0], "cd") == 0)
			&& current->next == NULL))
	{
		data->last_exit_status = execute_builtin(current, data, false);
		cleanup_data(data, false);
		free(ctx->child_pids);
		return (data->last_exit_status);
	}
	return (-1);
}

static int	execute_all_commands_in_list(t_cmd *current,
	t_data *data, t_exec_context *ctx)
{
	while (current != NULL)
	{
		if (current->io_error)
		{
			ctx->io_error_status = 1;
			current = current->next;
			continue ;
		}
		else
		{
			ctx->io_error_status = 0;
		}
		if (execute_command_in_list(current, data, ctx) == 1)
			return (1);
		current = current->next;
	}
	return (0);
}

int	execute_cmd_list(t_data *data)
{
	t_cmd			*current;
	t_exec_context	ctx;
	int				num_commands;
	int				builtin_status;

	ctx.prev_fd = -1;
	ctx.num_children = 0;
	ctx.io_error_status = 0;
	handle_heredoc(data->cmd_list, data);
	num_commands = count_cmds(data->cmd_list);
	ctx.child_pids = malloc(sizeof(pid_t) * num_commands);
	if (!ctx.child_pids)
		return (perror(": malloc"), 1);
	current = data->cmd_list;
	builtin_status = execute_single_builtin_if_needed(current, data, &ctx);
	if (builtin_status != -1)
		return (builtin_status);
	if (execute_all_commands_in_list(current, data, &ctx) == 1)
		return (cleanup_data(data, false), free(ctx.child_pids), 1);
	wait_for_children(ctx, data);
	cleanup_data(data, false);
	free(ctx.child_pids);
	if (ctx.io_error_status && data->last_exit_status == 0)
		data->last_exit_status = 1;
	return (data->last_exit_status);
}
