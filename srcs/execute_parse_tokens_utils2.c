/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_parse_tokens_utils2.c                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krabitsc <krabitsc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/21 07:31:36 by krabitsc          #+#    #+#             */
/*   Updated: 2024/10/21 09:23:03 by krabitsc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

int case_redirection(t_cmd *cur_cmd, int *arg_index, t_data *data)
{
	if (ft_strcmp(data->tokens->value, "<<") == 0)
	{
		cur_cmd->is_heredoc = true;
		if (data->tokens->next != NULL)
		{
			cur_cmd->heredoc_delim = ft_strdup(data->tokens->next->value);
			data->tokens = data->tokens->next;
		}
		else
		{
			ft_fprintf(2, "minishell: syntax error near unexpected token newline\n");
			data->last_exit_status = 2;
			return (-1);
		}
	}
	else if (ft_strcmp(data->tokens->value, ">") == 0)
	{
		if (data->tokens->next != NULL)
		{
			cur_cmd->output = ft_strdup(data->tokens->next->value);
			data->tokens = data->tokens->next;
			if (data->tokens->next != NULL)
			{
				if ((*arg_index == 0) && !cur_cmd->args[0])
				{
					cur_cmd->args[*arg_index++] = ft_strdup(data->tokens->next->value);
					data->tokens = data->tokens->next;
				}
			}
			else
			{
				if ((*arg_index == 0) && !cur_cmd->args[0])
					cur_cmd->args[*arg_index++] = ft_strdup("");
			}
		}
		else
		{
			ft_fprintf(2, "minishell: syntax error near unexpected token newline\n");
			data->last_exit_status = 2;
			return (-1);
		}
	}
	else if (ft_strcmp(data->tokens->value, ">>") == 0)
	{
		if (data->tokens->next != NULL)
		{
			cur_cmd->output = ft_strdup(data->tokens->next->value);
			cur_cmd->append_output = 1;
			data->tokens = data->tokens->next;
		}
		else
		{
			ft_fprintf(2, "minishell: syntax error near unexpected token newline\n");
			data->last_exit_status = 2;
			return (-1);
		}
	}
	else if (ft_strcmp(data->tokens->value, "<") == 0)
	{
		if (data->tokens->next != NULL)
		{
			if (access(data->tokens->next->value, F_OK) == 0)
			{
				cur_cmd->input = ft_strdup(data->tokens->next->value);
				data->tokens = data->tokens->next;
				if (data->tokens->next == NULL || data->tokens->next->type == TOKEN_OPERATOR)
				{
					if (data->tokens->next)
						ft_fprintf(2, "%s: command not found\n", data->tokens->next->value);
					else
						ft_fprintf(2, "command: command not found\n");
					return (-1);
				}
			}
			else
			{
				ft_fprintf(2, ": %s: No such file or directory\n", data->tokens->next->value);
				return (-1);
			}
			while (data->tokens && ft_strcmp(data->tokens->value, "|") != 0)
				data->tokens = data->tokens->next;
		}
		else
		{
			ft_fprintf(2, ": syntax error near unexpected token newline\n");
			data->last_exit_status = 2;
			return (-1);
		}
	}
	return (0);
}






/*
int handle_redirection_token(t_cmd *cur_cmd, t_data *data)
{
	if (ft_strcmp(data->tokens->value, "<<") == 0)
	{
		cur_cmd->is_heredoc = true;
		if (data->tokens->next != NULL)
		{
			cur_cmd->heredoc_delim = ft_strdup(data->tokens->next->value);
			data->tokens = data->tokens->next;
		}
		else
		{
			ft_fprintf(2, "minishell: syntax error near unexpected token newline\n");
			data->last_exit_status = 2;
			return (NULL);
		}
	}
	else if (ft_strcmp(data->tokens->value, ">") == 0)
	{
		if (data->tokens->next != NULL)
		{
			cur_cmd->output = ft_strdup(data->tokens->next->value);
			data->tokens = data->tokens->next;
			if (data->tokens->next != NULL)
			{
				if ((arg_index == 0) && !cur_cmd->args[0])
				{
					cur_cmd->args[arg_index++] = ft_strdup(data->tokens->next->value);
					data->tokens = data->tokens->next;
				}
			}
			else
			{
				if ((arg_index == 0) && !cur_cmd->args[0])
					cur_cmd->args[arg_index++] = ft_strdup("");
			}
		}
		else
		{
			ft_fprintf(2, "minishell: syntax error near unexpected token newline\n");
			data->last_exit_status = 2;
			return (NULL);
		}
	}
	else if (ft_strcmp(data->tokens->value, ">>") == 0)
	{
		if (data->tokens->next != NULL)
		{
			cur_cmd->output = ft_strdup(data->tokens->next->value);
			cur_cmd->append_output = 1;
			data->tokens = data->tokens->next;
		}
		else
		{
			ft_fprintf(2, "minishell: syntax error near unexpected token newline\n");
			data->last_exit_status = 2;
			return (NULL);
		}
	}
	else if (ft_strcmp(data->tokens->value, "<") == 0)
	{
		if (data->tokens->next != NULL)
		{
			if (access(data->tokens->next->value, F_OK) == 0)
			{
				cur_cmd->input = ft_strdup(data->tokens->next->value);
				data->tokens = data->tokens->next;
				if (data->tokens->next == NULL || data->tokens->next->type == TOKEN_OPERATOR)
				{
					if (data->tokens->next)
						ft_fprintf(2, "%s: command not found\n", data->tokens->next->value);
					else
						ft_fprintf(2, "command: command not found\n");
					return (NULL);
				}
			}
			else
			{
				ft_fprintf(2, ": %s: No such file or directory\n", data->tokens->next->value);
				return (NULL);
			}
			while (data->tokens && ft_strcmp(data->tokens->value, "|") != 0)
				data->tokens = data->tokens->next;
		}
		else
		{
			ft_fprintf(2, ": syntax error near unexpected token newline\n");
			data->last_exit_status = 2;
			return (NULL);
		}
	}
}
*/