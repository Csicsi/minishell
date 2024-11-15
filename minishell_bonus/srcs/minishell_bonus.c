/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell_bonus.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcsicsak <dcsicsak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/12 13:27:23 by dcsicsak          #+#    #+#             */
/*   Updated: 2024/11/15 17:52:12 by dcsicsak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell_bonus.h"

int	handle_null_input(t_data *data)
{
	if (data->input == NULL)
	{
		if (isatty(0))
			ft_fprintf(2, "exit\n");
		cleanup_data(data, true);
		return (1);
	}
	return (0);
}

int	check_for_unclosed_quotes(t_data *data)
{
	int		in_quote;
	char	quote_char;
	char	*cursor;

	cursor = data->input;
	in_quote = 0;
	quote_char = '\0';
	while (*cursor)
	{
		if ((*cursor == '"' || *cursor == '\'') && in_quote == 0)
		{
			in_quote = 1;
			quote_char = *cursor;
		}
		else if (*cursor == quote_char && in_quote == 1)
		{
			in_quote = 0;
		}
		cursor++;
	}
	return (in_quote);
}

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

int	process_and_validate_input(t_data *data)
{
	if (check_for_unclosed_quotes(data) != 0)
	{
		cleanup_data(data, false);
		return (1);
	}
	if (lexer(data) != 0)
	{
		cleanup_data(data, false);
		return (1);
	}
	validate_syntax(data);
	mark_error_on_pipe(data->tokens);
	data->cmd_list = parse_tokens(data);
	if (!data->cmd_list)
	{
		cleanup_data(data, false);
		return (1);
	}
	return (0);
}

int	main(int argc, char **argv, char **env_vars)
{
	t_data	data;
	char	*trimmed_input;

	if (initialize(&data, env_vars, argc, argv))
		return (1);
	while (1)
	{
		data.input = readline("Don'tPanicShell> ");
		if (handle_null_input(&data))
			return (data.last_exit_status);
		trimmed_input = ft_strtrim(data.input, " \t\n");
		if (!*trimmed_input)
		{
			free(trimmed_input);
			continue ;
		}
		free(trimmed_input);
		if (*data.input)
			add_history(data.input);
		if (process_and_validate_input(&data))
			continue ;
		data.last_exit_status = execute_cmd_list(&data);
		if (data.exit_flag)
			return (cleanup_data(&data, true), data.last_exit_status);
	}
}
