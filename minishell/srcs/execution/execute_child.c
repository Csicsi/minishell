/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_child.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcsicsak <dcsicsak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/12 13:43:16 by dcsicsak          #+#    #+#             */
/*   Updated: 2024/11/16 12:36:39 by dcsicsak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

static void	setup_pipes(t_exec_context *ctx, t_cmd *current)
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
}

static int	handle_redirections(t_cmd *current, t_data *data)
{
	if (current->redirection_order == OUTPUT_FIRST)
	{
		if (handle_output_redirection(current, data) < 0
			|| handle_input_redirection(current, data) < 0)
			return (-1);
	}
	else
	{
		if (handle_input_redirection(current, data) < 0
			|| handle_output_redirection(current, data) < 0)
			return (-1);
	}
	return (0);
}

static void	execute_child_command(t_cmd *current,
	t_data *data, t_exec_context *ctx)
{
	setup_pipes(ctx, current);
	if (handle_redirections(current, data) < 0)
	{
		cleanup_data(data, true);
		free(ctx->child_pids);
		exit(data->last_exit_status);
	}
	if (is_builtin(current->args[0]))
		data->last_exit_status = execute_builtin(current, data, ctx, false);
	else
		data->last_exit_status = execute_single_cmd(current, data, ctx);
	cleanup_data(data, true);
	free(ctx->child_pids);
	exit(data->last_exit_status);
}

int	execute_command_in_list(t_cmd *current, t_data *data,
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
		setup_signal_handlers(1);
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
