/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   initialize.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krabitsc <krabitsc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/20 12:04:47 by krabitsc          #+#    #+#             */
/*   Updated: 2024/11/18 13:22:45 by krabitsc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

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

static pid_t	ft_parse_pid_from_stat(const char *buffer)
{
	const char	*token = buffer;
	int			field_count;
	pid_t		parent_pid;

	field_count = 0;
	while (*token != '\0' && field_count < 4)
	{
		if (*token == ' ')
			field_count++;
		token++;
	}
	if (field_count < 4 || *token == '\0')
		return (-1);
	parent_pid = (pid_t)ft_atoi(token);
	return (parent_pid);
}

static pid_t	ft_getpid(void)
{
	int		fd;
	char	buffer[256];
	ssize_t	bytes_read;

	fd = open("/proc/self/stat", O_RDONLY);
	if (fd == -1)
		return (-1);
	bytes_read = read(fd, buffer, sizeof(buffer) - 1);
	close(fd);
	if (bytes_read <= 0)
		return (-1);
	buffer[bytes_read] = '\0';
	if (bytes_read <= 0)
		return (-1);
	return (ft_parse_pid_from_stat(buffer));
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
