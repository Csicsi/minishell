/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_child_bonus.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcsicsak <dcsicsak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/12 13:43:16 by dcsicsak          #+#    #+#             */
/*   Updated: 2024/11/18 13:09:00 by dcsicsak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell_bonus.h"

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

int	execute_cmd_group(t_cmd_group *group, t_data *data, t_exec_context *ctx)
{
	while (group != NULL)
	{
		if (execute_all_commands_in_list(group->cmd_list, data, ctx) == 1)
			return (1);
		group = group->next_group;
	}
	return (data->last_exit_status);
}

static void	execute_child_command(t_cmd *current,
	t_data *data, t_exec_context *ctx)
{
	setup_signal_handlers(2);
	setup_pipes(ctx, current);
	if (handle_redirections(current, data) < 0)
	{
		cleanup_data(data, true);
		free(ctx->child_pids);
		exit(data->last_exit_status);
	}
	if (current->group != NULL)
		data->last_exit_status = execute_cmd_group(current->group, data, ctx);
	else if (is_builtin(current->args[0]))
		data->last_exit_status = execute_builtin(current, data, ctx, false);
	else
		data->last_exit_status = execute_single_cmd(current, data, ctx);
	cleanup_data(data, true);
	free(ctx->child_pids);
	exit(data->last_exit_status);
}

void	handle_parent_process(pid_t pid, t_cmd *current, t_exec_context *ctx)
{
	setup_signal_handlers(1);
	ctx->child_pids[ctx->num_children++] = pid;
	if (ctx->prev_fd != -1)
		close(ctx->prev_fd);
	if (current->next != NULL && (current->type == CMD_NORMAL
			|| current->type == CMD_PIPE))
	{
		close(ctx->pipe_fd[1]);
		ctx->prev_fd = ctx->pipe_fd[0];
	}
	else
		ctx->prev_fd = -1;
}

int	execute_command_in_list(t_cmd *current, t_data *data,
	t_exec_context *ctx)
{
	pid_t	pid;

	if (current->next != NULL && (current->type == CMD_NORMAL
			|| current->type == CMD_PIPE) && (pipe(ctx->pipe_fd) < 0))
		return (cleanup_data(data, false), free(ctx->child_pids), 1);
	pid = fork();
	if (pid == 0)
		execute_child_command(current, data, ctx);
	else if (pid > 0)
		handle_parent_process(pid, current, ctx);
	else
		return (perror(": fork"), 1);
	return (0);
}
