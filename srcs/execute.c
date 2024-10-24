#include "../includes/minishell.h"

int execute_single_cmd(t_cmd *cmd, t_data *data)
{
	int fd_in = -1;
	int fd_out = -1;
	char *cmd_path;
	int i = 0;

	if (cmd->input != NULL)
	{
		fd_in = open(cmd->input, O_RDONLY);
		if (fd_in < 0)
		{
			ft_fprintf(2, ": %s: No such file or directory\n", cmd->input);
			data->last_exit_status = 1;
			return (1);
		}
		dup2(fd_in, STDIN_FILENO);
		close(fd_in);
	}
	if (cmd->is_heredoc)
	{
		fd_in = handle_heredoc(cmd);
		dup2(fd_in, STDIN_FILENO);
		close(fd_in);
	}
	if (cmd->output != NULL)
	{
		if (cmd->append_output)
			fd_out = open(cmd->output, O_WRONLY | O_CREAT | O_APPEND, 0644);
		else
			fd_out = open(cmd->output, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if (fd_out < 0)
		{
			ft_fprintf(2, ": %s: Cannot open file\n", cmd->output);
			data->last_exit_status = 1;
			return (1);
		}
		dup2(fd_out, STDOUT_FILENO);
		close(fd_out);
	}
	if (is_builtin(cmd->args[i]))
	{
		data->last_exit_status = execute_builtin(cmd, data, false);
		return data->last_exit_status;
	}
	cmd_path = find_cmd_path(cmd->args + i, data);
	if (!cmd_path)
	{
		if (ft_strchr(cmd->args[i], '/'))
		{
			ft_fprintf(2, ": %s: No such file or directory\n", cmd->args[i]);
			data->last_exit_status = 127;
		}
		else
		{
			ft_fprintf(2, ": %s: command not found\n", cmd->args[i]);
			data->last_exit_status = 127;
		}
		return (data->last_exit_status);
	}
	if (update_last_command_env_var(data, cmd_path) == -1)
	{
		free(cmd_path);
		data->last_exit_status = 1;
		return (1);
	}
	if (execve(cmd_path, cmd->args + i, data->env_vars) == -1)
	{
		if (ft_strchr(cmd->args[i], '/'))
		{
			ft_fprintf(2, ": %s: Is a directory\n", cmd->args[i]);
			data->last_exit_status = 126;
		}
		else
		{
			ft_fprintf(2, ": %s: command not found\n", cmd->args[i]);
			data->last_exit_status = 127;
		}
		free(cmd_path);
		return data->last_exit_status;
	}
	free(cmd_path);
	return (0);
}

int ft_isdigit_str(const char *str)
{
	int i = 0;

	if (!str || str[0] == '\0')
		return (0);
	while (str[i])
	{
		if (!ft_isdigit(str[i]))
			return (0);
		i++;
	}
	return (1);
}

int execute_cmd_list(t_data *data)
{
	t_cmd *current;
	int pipe_fd[2];
	int prev_fd = -1;
	pid_t pid;
	int num_children = 0;
	int num_commands = count_cmds(data->cmd_list);
	pid_t *child_pids;
	int i;
	int io_error_status = 0;

	child_pids = malloc(sizeof(pid_t) * num_commands);
	if (!child_pids)
	{
		perror(": malloc");
		return (1);
	}
	current = data->cmd_list;

	if (current->args[0] && ft_strcmp(current->args[0], "exit") == 0 && current->next == NULL)
	{
		data->last_exit_status = execute_builtin(current, data, true);
		cleanup_data(data, true);
		free(child_pids);
		return (data->last_exit_status);
	}

	if (current->args[0] != NULL && ((ft_strcmp(current->args[0], "export") == 0 ||
									  ft_strcmp(current->args[0], "unset") == 0 ||
									  ft_strcmp(current->args[0], "cd") == 0) && current->next == NULL))
	{
		data->last_exit_status = execute_builtin(current, data, false);
		return data->last_exit_status;
	}

	while (current != NULL)
	{
		if (current->io_error)
		{
			io_error_status = 1;
			current = current->next;
			continue;
		}
		else
			io_error_status = 0;
		if (current->next != NULL)
			pipe(pipe_fd);

		pid = fork();
		if (pid == 0)
		{
			if (prev_fd != -1)
			{
				dup2(prev_fd, STDIN_FILENO);
				close(prev_fd);
			}
			if (current->next != NULL)
			{
				dup2(pipe_fd[1], STDOUT_FILENO);
				close(pipe_fd[0]);
				close(pipe_fd[1]);
			}

			if (current->output != NULL)
			{
				int fd_out;
				if (current->append_output)
					fd_out = open(current->output, O_WRONLY | O_CREAT | O_APPEND, 0644);
				else
					fd_out = open(current->output, O_WRONLY | O_CREAT | O_TRUNC, 0644);
				if (fd_out < 0)
				{
					ft_fprintf(2, "%s: Permission denied\n", current->output);
					exit(1);
				}
				dup2(fd_out, STDOUT_FILENO);
				close(fd_out);
			}

			if (is_builtin(current->args[0]))
				data->last_exit_status = execute_builtin(current, data, false);
			else
				data->last_exit_status = execute_single_cmd(current, data);

			cleanup_data(data, true);
			free(child_pids);
			exit(data->last_exit_status);
		}
		else if (pid > 0)
		{
			child_pids[num_children++] = pid;
			if (prev_fd != -1)
				close(prev_fd);
			if (current->next != NULL)
			{
				close(pipe_fd[1]);
				prev_fd = pipe_fd[0];
			}
			else
				prev_fd = -1;
		}
		else
		{
			perror(": fork");
			cleanup_data(data, true);
			free(child_pids);
			return (1);
		}

		current = current->next;
	}

	i = 0;
	while (i < num_children)
	{
		waitpid(child_pids[i], &data->last_exit_status, 0);
		if (WIFEXITED(data->last_exit_status))
			data->last_exit_status = WEXITSTATUS(data->last_exit_status);
		i++;
	}

	cleanup_data(data, false);
	free(child_pids);

	if (io_error_status && data->last_exit_status == 0)
		data->last_exit_status = 1;

	return (data->last_exit_status);
}

void check_input_error(t_cmd *cmd, t_data *data)
{
    if (cmd->input && access(cmd->input, F_OK) != 0)
    {
        ft_fprintf(2, ": %s: No such file or directory\n", cmd->input);
        cmd->io_error = true;
        data->last_exit_status = 1;
    }
}

void check_output_error(t_cmd *cmd, t_data *data)
{
    char *output_dir = get_directory_from_path(cmd->output);

    if (cmd->output && output_dir && access(output_dir, F_OK) != 0)
    {
        ft_fprintf(2, ": %s: No such file or directory\n", cmd->output);
        cmd->io_error = true;
        data->last_exit_status = 1;
    }
    free(output_dir);
}

int validate_cmd_list(t_cmd *cmd_list, t_data *data)
{
	t_cmd *current = cmd_list;

	while (current != NULL)
	{
		current->io_error = false;

		if (current->io_flag == IO_INPUT_FIRST)
		{
			check_input_error(current, data);
			if (!current->io_error)
				check_output_error(current, data);
		}
		else if (current->io_flag == IO_OUTPUT_FIRST)
		{
			check_output_error(current, data);
			if (!current->io_error)
				check_input_error(current, data);
		}
		else
		{
			check_input_error(current, data);
			check_output_error(current, data);
		}

		current = current->next;
	}

	return 0;
}

int validate_syntax(t_token *tokens, t_data *data)
{
	t_token *current = tokens;

	if (tokens && tokens->type == TOKEN_OPERATOR && ft_strcmp(tokens->value, "|") == 0)
	{
		ft_fprintf(2, "syntax error near unexpected token `%s'\n", tokens->value);
		data->last_exit_status = 2;
		return (-1);
	}

	while (current)
	{
		if (current->type == TOKEN_OPERATOR)
		{
			if ((ft_strcmp(current->value, "|") == 0 || ft_strcmp(current->value, ">") == 0 ||
				 ft_strcmp(current->value, ">>") == 0 || ft_strcmp(current->value, "<") == 0 ||
				 ft_strcmp(current->value, "<<") == 0) && !current->next)
			{
				ft_fprintf(2, "syntax error near unexpected token `newline'\n");
				data->last_exit_status = 2;
				return (-1);
			}
			if (ft_strcmp(current->value, "|") != 0 && current->next && current->next->type == TOKEN_OPERATOR)
			{
				ft_fprintf(2, "syntax error near unexpected token `%s'\n", current->next->value);
				data->last_exit_status = 2;
				return (-1);
			}
			if (ft_strcmp(current->value, "|") == 0 && !current->next)
			{
				ft_fprintf(2, "syntax error near unexpected token `|'\n");
				data->last_exit_status = 2;
				return (-1);
			}
		}
		current = current->next;
	}
	data->last_exit_status = 0;
	return (0);
}

int main(int argc, char **argv, char **env_vars)
{
	t_data data;

	if (initialize(&data, env_vars, argc, argv))
		return (1);
	while (1)
	{
		if (isatty(0))
		{
			data.input = readline("Don'tPanicShell> ");
		}
		else
		{
			data.input = get_next_line(0);
			if (data.input == NULL)
			{
				cleanup_data(&data, true);
				return (data.last_exit_status);
			}
			size_t len = ft_strlen(data.input);
			if (len > 0 && data.input[len - 1] == '\n')
				data.input[len - 1] = '\0';
		}
		if (data.input == NULL)
		{
			if (isatty(0))
				ft_fprintf(2, "exit\n");
			cleanup_data(&data, true);
			return (data.last_exit_status);
		}
		if (isatty(0) && *data.input)
			add_history(data.input);
		if (check_for_unclosed_quotes(data.input))
		{
			ft_fprintf(2, "syntax error: unclosed quote\n");
			cleanup_data(&data, false);
			continue;
		}
		if (lexer(data.input, &data, data.last_exit_status) == -1)
		{
			cleanup_data(&data, false);
			continue;
		}
		if (validate_syntax(data.tokens, &data) == -1)
		{
			cleanup_data(&data, false);
			continue;
		}
		data.cmd_list = parse_tokens(&data);
		if (!data.cmd_list)
		{
			cleanup_data(&data, false);
			continue;
		}
		if (validate_cmd_list(data.cmd_list, &data) != 0)
		{
			cleanup_data(&data, false);
			continue;
		}
		data.last_exit_status = execute_cmd_list(&data);
		if (data.exit_flag == true)
			return (cleanup_data(&data, true), data.last_exit_status);
	}
	return (cleanup_data(&data, true), 0);
}
