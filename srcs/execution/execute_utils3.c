/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_utils3.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcsicsak <dcsicsak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/21 07:31:36 by krabitsc          #+#    #+#             */
/*   Updated: 2024/10/31 13:40:11 by dcsicsak         ###   ########.fr       */
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

char *generate_random_filename()
{
	unsigned char	random_bytes[20];
	const char		charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	int				urandom_fd;
	char			*filename;
	int				i;

	urandom_fd = open("/dev/urandom", O_RDONLY);
	if (urandom_fd < 0)
	{
		perror("open /dev/urandom");
		exit(1);
	}
	filename = malloc(20);
	if (!filename)
	{
		perror("malloc");
		close(urandom_fd);
		exit(1);
	}
	if (read(urandom_fd, random_bytes, 19) != 19)
	{
		perror("read /dev/urandom");
		free(filename);
		close(urandom_fd);
		exit(1);
	}
	i = 0;
	while (i < 19)
	{
		filename[i] = charset[random_bytes[i] % (sizeof(charset) - 1)];
		i++;
	}
	filename[19] = '\0';
	close(urandom_fd);

	return filename;
}

void handle_heredoc(t_cmd *cmd_list, t_data *data)
{
	t_cmd *current;
	char *line;
	char *expanded_line;
	int fd, len;
	bool has_heredoc = false;

	current = cmd_list;
	while (current)
	{
		if (current->is_heredoc)
		{
			has_heredoc = true;
			current->heredoc_tempfile = generate_random_filename();
			if (!current->heredoc_tempfile)
			{
				perror("generate_random_filename");
				exit(1);
			}
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
				if (!data->heredoc_single_quote)
				{
					expanded_line = expand_env_var_in_str(&line, data->last_exit_status, data);
					if (!expanded_line)
					{
						perror("expand_env_var_in_str");
						free(line);
						close(fd);
						return;
					}
					write(fd, expanded_line, ft_strlen(expanded_line));
					free(expanded_line);
				}
				else
					write(fd, line, ft_strlen(line));
			}
			close(fd);
		}
		current = current->next;
	}
	if (has_heredoc)
		validate_cmd_list(data);
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
