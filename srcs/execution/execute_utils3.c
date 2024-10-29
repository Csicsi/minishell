/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_utils3.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: csicsi <csicsi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/21 07:31:36 by krabitsc          #+#    #+#             */
/*   Updated: 2024/10/29 16:05:20 by csicsi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

int	is_builtin(char *command_name)
{
	if (!command_name)
		return (0);
	if (ft_strncmp(command_name, "cd", 3) == 0)
		return (1);
	else if (ft_strncmp(command_name, "echo", 5) == 0)
		return (1);
	else if (ft_strncmp(command_name, "exit", 5) == 0)
		return (1);
	else if (ft_strncmp(command_name, "env", 4) == 0)
		return (1);
	else if (ft_strncmp(command_name, "export", 7) == 0)
		return (1);
	else if (ft_strncmp(command_name, "unset", 6) == 0)
		return (1);
	else if (ft_strncmp(command_name, "pwd", 4) == 0)
		return (1);
	return (0);
}

int	execute_builtin(t_cmd *cmd, t_data *data, bool print_exit)
{
	if (ft_strncmp(cmd->args[0], "cd", 3) == 0)
		return (builtin_cd(cmd, data));
	else if (ft_strncmp(cmd->args[0], "echo", 5) == 0)
		return (builtin_echo(cmd));
	else if (ft_strncmp(cmd->args[0], "exit", 5) == 0)
		return (builtin_exit(cmd, data, print_exit));
	else if (ft_strncmp(cmd->args[0], "env", 4) == 0)
		return (builtin_env(data));
	else if (ft_strncmp(cmd->args[0], "export", 7) == 0)
		return (builtin_export(cmd, data));
	else if (ft_strncmp(cmd->args[0], "unset", 6) == 0)
		return (builtin_unset(cmd, data));
	else if (ft_strncmp(cmd->args[0], "pwd", 4) == 0)
		return (builtin_pwd());
	return (1);
}

void handle_heredoc(t_cmd *cmd_list)
{
	const char *preset_filenames[] = {
		"heredoc_1.tmp",
		"heredoc_2.tmp",
		"heredoc_3.tmp",
		"heredoc_4.tmp",
		"heredoc_5.tmp"
	};
	int preset_count = sizeof(preset_filenames) / sizeof(preset_filenames[0]);
	int current_preset_index = 0;
	t_cmd	*current;
	char	*line;
	int		fd, len;

	current = cmd_list;
	while (current)
	{
		if (current->is_heredoc)
		{
			current->heredoc_tempfile = malloc(20);
			if (!current->heredoc_tempfile)
			{
				perror("malloc");
				exit(1);
			}
			snprintf(current->heredoc_tempfile, 20, "%s", preset_filenames[current_preset_index]);
			current_preset_index = (current_preset_index + 1) % preset_count;
			fd = open(current->heredoc_tempfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
			if (fd == -1)
			{
				perror("open");
				free(current->heredoc_tempfile);
				return;
			}
			len = ft_strlen(current->heredoc_delim);
			while (1)
			{
				if (isatty(STDIN_FILENO))
					write(STDOUT_FILENO, "> ", 2);
				line = get_next_line(STDIN_FILENO);
				if (!line || (ft_strncmp(line, current->heredoc_delim, len) == 0 && line[len] == '\n'))
				{
					free(line);
					break;
				}
				write(fd, line, ft_strlen(line));
				free(line);
			}
			close(fd);
		}
		current = current->next;
	}
}

static char	*find_in_path(char *cmd, t_data *data)
{
	int		i;
	char	**allpath;
	char	*path_env;
	char	*path_for_execve;

	path_env = ft_getenv(ft_strdup("PATH"), data->env_vars);
	if (!path_env)
		return (NULL);
	allpath = ft_split(path_env, ':');
	i = -1;
	while (allpath[++i])
	{
		path_for_execve = ft_strjoin_pipex(allpath[i], cmd);
		if (!path_for_execve)
			return (free_string_array(allpath), NULL);
		if (access(path_for_execve, F_OK | X_OK) == 0)
		{
			free_string_array(allpath);
			return (path_for_execve);
		}
		path_for_execve = free_null(path_for_execve);
	}
	free_string_array(allpath);
	return (NULL);
}

char	*find_cmd_path(char **cmd_args, t_data *data)
{
	if (!cmd_args || !cmd_args[0])
		return (NULL);
	if (ft_strchr(cmd_args[0], '/') != NULL)
	{
		if (access(cmd_args[0], F_OK | X_OK) != 0)
			return (NULL);
		return (ft_strdup(cmd_args[0]));
	}
	if (access(cmd_args[0], F_OK | X_OK) == 0)
		return (ft_strdup(cmd_args[0]));
	return (find_in_path(cmd_args[0], data));
}
