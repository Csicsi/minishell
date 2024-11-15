/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_list_bonus.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcsicsak <dcsicsak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/12 13:25:03 by dcsicsak          #+#    #+#             */
/*   Updated: 2024/11/15 13:01:36 by dcsicsak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell_bonus.h"

static int	handle_errors(t_data *data, t_exec_context *ctx, t_cmd *current)
{
	if (data->syntax_error)
	{
		cleanup_data(data, false);
		free(ctx->child_pids);
		return (2);
	}
	if (current->empty_redir)
	{
		cleanup_data(data, false);
		free(ctx->child_pids);
		return (1);
	}
	return (-1);
}

static int	execute_single_builtin_if_needed(t_cmd *current,
	t_data *data, t_exec_context *ctx)
{
	int	ret;

	ret = handle_errors(data, ctx, current);
	if (ret != -1)
		return (ret);
	if (current->args[0] && ft_strcmp(current->args[0], "exit") == 0
		&& current->next == NULL)
	{
		data->last_exit_status = execute_builtin(current, data, ctx, true);
		cleanup_data(data, true);
		free(ctx->child_pids);
		return (data->last_exit_status);
	}
	if (current->args[0] != NULL && ((ft_strcmp(current->args[0], "export") == 0
				|| ft_strcmp(current->args[0], "unset") == 0
				|| ft_strcmp(current->args[0], "cd") == 0)
			&& current->next == NULL))
	{
		data->last_exit_status = execute_builtin(current, data, ctx, false);
		cleanup_data(data, false);
		free(ctx->child_pids);
		return (data->last_exit_status);
	}
	return (-1);
}

int	execute_all_commands_in_list(t_cmd *current,
	t_data *data, t_exec_context *ctx)
{
	t_cmd_type	last_cmd_type;
	int			status;
	bool		skip_piped_group;

	last_cmd_type = CMD_NORMAL;
	skip_piped_group = false;
	while (current != NULL)
	{
		if (skip_piped_group)
		{
			if (last_cmd_type == CMD_PIPE)
			{
				current = current->next;
				continue ;
			}
			skip_piped_group = false;
		}
		if ((last_cmd_type == CMD_AND && data->last_exit_status != 0)
			|| (last_cmd_type == CMD_OR && data->last_exit_status == 0))
		{
			skip_piped_group = true;
			last_cmd_type = current->type;
			current = current->next;
			continue ;
		}
		if (data->syntax_error || current->io_error)
		{
			current->skip_execution = true;
			current = current->next;
			continue ;
		}
		if (execute_command_in_list(current, data, ctx) == 1)
			return (1);
		if (waitpid(ctx->child_pids[ctx->num_children - 1], &status, 0) > 0)
		{
			if (WIFEXITED(status))
				data->last_exit_status = WEXITSTATUS(status);
		}
		last_cmd_type = current->type;
		current = current->next;
	}
	return (0);
}

int	initialize_exec_context(t_data *data, t_exec_context *ctx)
{
	int	num_commands;

	ctx->prev_fd = -1;
	ctx->num_children = 0;
	ctx->io_error_status = 0;
	ctx->path_exists = true;
	num_commands = count_cmds(data->cmd_list);
	ctx->child_pids = malloc(sizeof(pid_t) * num_commands);
	if (!ctx->child_pids)
	{
		perror(": malloc");
		return (1);
	}
	return (0);
}

int	execute_cmd_list(t_data *data)
{
	t_cmd			*current;
	t_exec_context	ctx;
	int				builtin_status;

	handle_heredoc(data->cmd_list, data);
	if (initialize_exec_context(data, &ctx) != 0)
		return (1);
	current = data->cmd_list;
	builtin_status = execute_single_builtin_if_needed(current, data, &ctx);
	if (builtin_status != -1)
		return (builtin_status);
	if (execute_all_commands_in_list(current, data, &ctx) == 1)
		return (cleanup_data(data, false), free(ctx.child_pids), 1);
	cleanup_data(data, false);
	free(ctx.child_pids);
	if (ctx.io_error_status && data->last_exit_status == 0)
		data->last_exit_status = 1;
	return (data->last_exit_status);
}
