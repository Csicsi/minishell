/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   initialize_bonus.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcsicsak <dcsicsak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/20 12:04:47 by krabitsc          #+#    #+#             */
/*   Updated: 2024/11/18 13:15:12 by dcsicsak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell_bonus.h"

static char	*duplicate_env_var(char *env_var)
{
	int		shlvl_value;
	char	*shlvl_value_str;
	char	*shlvl_str;

	if (ft_strncmp(env_var, "SHLVL=", 6) == 0)
	{
		shlvl_value = ft_atoi(env_var + 6);
		shlvl_value++;
		shlvl_value_str = ft_itoa(shlvl_value);
		if (!shlvl_value_str)
			return (NULL);
		shlvl_str = ft_strjoin("SHLVL=", shlvl_value_str);
		free(shlvl_value_str);
		return (shlvl_str);
	}
	return (ft_strdup(env_var));
}

char	**duplicate_env_vars(char **env_vars)
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
		new_env_vars[i] = duplicate_env_var(env_vars[i]);
		if (!new_env_vars[i])
		{
			while (i-- > 0)
				free(new_env_vars[i]);
			return (free(new_env_vars), NULL);
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
	setup_signal_handlers(0);
	data->exit_flag = false;
	data->syntax_error = false;
	data->heredoc_single_quote = false;
	data->last_exit_status = 0;
	data->token_count = 0;
	data->input = NULL;
	data->env_vars = NULL;
	data->tokens = NULL;
	data->cmd_list = NULL;
	data->pid = ft_getpid();
	data->env_vars = duplicate_env_vars(env_vars);
	if (!data->env_vars)
	{
		ft_fprintf(2, "Error allocating memory for env_vars\n");
		data->exit_flag = true;
	}
	return (data->exit_flag);
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
