/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell_bonus.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcsicsak <dcsicsak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/12 13:27:23 by dcsicsak          #+#    #+#             */
/*   Updated: 2024/11/16 07:19:34 by dcsicsak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell_bonus.h"

int	check_for_unclosed_single_quotes(t_data *data)
{
	int		in_quote;
	char	quote_char;
	char	*cursor;

	cursor = data->input;
	in_quote = 0;
	quote_char = '\0';
	while (*cursor)
	{
		if ((*cursor == '\'') && in_quote == 0)
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
	if (in_quote == 1)
		ft_fprintf(2, "syntax error near unexpected token `\''\n");
	return (in_quote);
}

int	check_for_unclosed_double_quotes(t_data *data)
{
	int		in_quote;
	char	quote_char;
	char	*cursor;

	cursor = data->input;
	in_quote = 0;
	quote_char = '\0';
	while (*cursor)
	{
		if ((*cursor == '"') && in_quote == 0)
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
	if (in_quote == 1)
		ft_fprintf(2, "syntax error near unexpected token `\"'\n");
	return (in_quote);
}

int	process_and_validate_input(t_data *data)
{
	if (check_for_unclosed_single_quotes(data) != 0)
	{
		cleanup_data(data, false);
		return (1);
	}
	if (check_for_unclosed_double_quotes(data) != 0)
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
