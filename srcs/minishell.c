#include "../includes/minishell.h"

int	process_step(t_data *data, int (*func)(t_data *))
{
	if (func(data) != 0)
	{
		cleanup_data(data, false);
		return (1);
	}
	return (0);
}

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

int	process_and_validate_input(t_data *data)
{
	if (process_step(data, check_for_unclosed_quotes))
		return (1);
	if (process_step(data, check_for_brackets))
		return (1);
	if (process_step(data, lexer))
		return (1);
	if (process_step(data, validate_syntax))
		return (1);
	data->cmd_list = parse_tokens(data);
	if (!data->cmd_list)
	{
		cleanup_data(data, false);
		return (1);
	}
	return (0);
}

/*
//for testing leaks
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
}*/

//for normal testing
int	main(int argc, char **argv, char **env_vars)
{
	t_data	data;
	char	*trimmed_input;
	size_t	len;

	if (initialize(&data, env_vars, argc, argv))
		return (1);
	while (1)
	{
		if (isatty(0))
			data.input = readline("Don'tPanicShell> ");
		else
		{
			data.input = get_next_line(0);
			if (data.input == NULL)
			{
				cleanup_data(&data, true);
				return (data.last_exit_status);
			}
			len = ft_strlen(data.input);
			if (len > 0 && data.input[len - 1] == '\n')
				data.input[len - 1] = '\0';
		}
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
