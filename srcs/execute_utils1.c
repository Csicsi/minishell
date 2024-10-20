/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_utils1.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krabitsc <krabitsc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/20 12:04:47 by krabitsc          #+#    #+#             */
/*   Updated: 2024/10/20 13:19:06 by krabitsc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

static void	handle_sigint(int sig)
{
	(void)sig;
	write(STDOUT_FILENO, "\n", 1);
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
}

static char	**duplicate_env_vars(char **env_vars)
{
	int		i;
	char	**new_env_vars;

	i = 0;
	while (env_vars[i] != NULL)
		i++;
	new_env_vars = (char **)malloc((i + 1) * sizeof(char *));
	if (!new_env_vars)
		return (NULL);
	i = 0;
	while (env_vars[i] != NULL)
	{
		new_env_vars[i] = ft_strdup(env_vars[i]);
		if (!new_env_vars[i])
		{
			while (i-- > 0)
				free(new_env_vars[i]);
			free(new_env_vars);
			return (NULL);
		}
		i++;
	}
	new_env_vars[i] = NULL;
	return (new_env_vars);
}

bool	initialize(t_data *data, char **env_vars, int argc, char **argv)
{
	(void)argc;
	(void)argv;
	signal(SIGINT, handle_sigint);
	data->exit_flag = false;
	data->last_exit_status = 0;
	data->token_count = 0;
	data->input = NULL;
	data->env_vars = NULL;
	data->tokens = NULL;
	data->cmd_list = NULL;
	data->env_vars = duplicate_env_vars(env_vars);
	if (!data->env_vars)
	{
		printf("Error allocating memory for env_vars\n");
		data->exit_flag = true;
	}
	return (data->exit_flag);
}

static int	check_for_unclosed_quotes(char *cursor)
{
	int		in_quote;
	char	quote_char;

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

char	*get_input_line(t_data *data)
{
	int		in_quote;
	char	*input;

	input = readline("Don'tPanicShell> ");
	if (input == NULL)
		return (NULL);
	if (*input)
		add_history(input);
	in_quote = check_for_unclosed_quotes(input);
	if (in_quote == 1)
	{
		ft_fprintf(2, "syntax error: unclosed quote\n");
		free(input);
		return (NULL);
	}
	data->input = input;
	return (input);
}
