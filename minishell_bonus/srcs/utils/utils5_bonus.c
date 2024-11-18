/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils5_bonus.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcsicsak <dcsicsak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/16 12:09:52 by dcsicsak          #+#    #+#             */
/*   Updated: 2024/11/18 09:58:09 by dcsicsak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell_bonus.h"

void	mark_error_on_pipe(t_token *tokens)
{
	t_token	*last_pipe;

	last_pipe = tokens;
	while (tokens)
	{
		if (tokens->type == TOKEN_OPERATOR
			&& ft_strcmp(tokens->value, "|") == 0)
			last_pipe = tokens;
		else if (tokens->type == TOKEN_ERROR)
		{
			last_pipe->type = TOKEN_ERROR;
			break ;
		}
		tokens = tokens->next;
	}
}

void	setup_pipes(t_exec_context *ctx, t_cmd *current)
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

int	handle_special_variables(char *result, int *i,
	char cursor, t_data *data)
{
	char	*temp_str;

	if (cursor == '?')
		temp_str = ft_itoa(data->last_exit_status);
	else if (cursor == '$')
		temp_str = ft_itoa(data->pid);
	else
		return (0);
	ft_strcpy(&result[*i], temp_str);
	*i += ft_strlen(temp_str);
	free(temp_str);
	return (1);
}
