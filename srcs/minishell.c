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
	if (!check_for_heredoc(data->tokens) && validate_cmd_list(data))
	{
		cleanup_data(data, false);
		return (1);
	}
	return (0);
}

char	*ft_strtrim(const char *str, const char *set)
{
	size_t	start;
	size_t	end;
	char	*trimmed;

	if (!str || !set)
		return (NULL);
	start = 0;
	while (str[start] && ft_strchr(set, str[start]))
		start++;
	end = ft_strlen(str);
	while (end > start && ft_strchr(set, str[end - 1]))
		end--;
	trimmed = (char *)malloc(sizeof(char) * (end - start + 1));
	if (!trimmed)
		return (NULL);
	ft_strlcpy(trimmed, str + start, end - start + 1);
	trimmed[end - start] = '\0';
	return (trimmed);
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
