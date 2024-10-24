#include "../includes/minishell.h"

int	execute_single_cmd(t_cmd *cmd, t_data *data)
{
	int		fd_in;
	int		fd_out;
	char	*cmd_path;
	int		i;

	fd_in = -1;
	fd_out = -1;
	i = 0;
	if (cmd->input != NULL)
	{
		fd_in = open(cmd->input, O_RDONLY);
		if (fd_in < 0)
		{
			ft_fprintf(2, ": %s: ", cmd->input);
			perror("");
			return (1);
		}
		dup2(fd_in, STDIN_FILENO);
		close(fd_in);
	}
	if (cmd->is_heredoc)
	{
		fd_in = handle_heredoc(cmd);
		if (fd_in == -1)
		{
			return (1);
		}
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
			ft_fprintf(2, ": %s: ", cmd->output);
			perror("");
			return (1);
		}
		dup2(fd_out, STDOUT_FILENO);
		close(fd_out);
	}
	if (is_builtin(cmd->args[i]))
		return (execute_builtin(cmd, data, false));
	cmd_path = find_cmd_path(cmd->args + i, data);
	if (!cmd_path)
	{
		if (ft_strchr(cmd->args[i], '/'))
			ft_fprintf(2, ": %s: No such file or directory\n", cmd->args[i]);
		else
			ft_fprintf(2, ": %s: command not found\n", cmd->args[i]);
		cleanup_data(data, true);
		return (127);
	}
	if (update_last_command_env_var(data, cmd_path) == -1)
	{
		free(cmd_path);
		cleanup_data(data, true);
		return (1);
	}
	if (execve(cmd_path, cmd->args + i, data->env_vars) == -1)
	{
		if (ft_strchr(cmd->args[i], '/'))
		{
			ft_fprintf(2, ": %s: Is a directory\n", cmd->args[i]);
			cleanup_data(data, true);
			return (126);
		}
		else
		{
			ft_fprintf(2, ": %s: command not found\n", cmd->args[i]);
			cleanup_data(data, true);
			return (127);
		}
	}
	free(cmd_path);
	cleanup_data(data, true);
	exit(EXIT_FAILURE);
}

int	execute_cmd_list(t_data *data)
{
	t_cmd	*current;
	int			pipe_fd[2];
	int			prev_fd;
	pid_t		pid;
	int			num_children;
	int			num_commands;
	pid_t		*child_pids;
	int			i;
	int			fd_out;
	char		*output_dir;

	num_children = 0;
	num_commands = count_cmds(data->cmd_list);
	child_pids = malloc(sizeof(pid_t) * num_commands);
	if (!child_pids)
	{
		perror(": malloc");
		return (1);
	}
	current = data->cmd_list;
	prev_fd = -1;
	if (current->args[0] && ft_strcmp(current->args[0], "exit") == 0 && current->next == NULL)
	{
		data->last_exit_status = execute_builtin(current, data, true);
		cleanup_data(data, true);
		free(child_pids);
		return (data->last_exit_status);
	}
	if (current->args[0] != NULL && ((ft_strcmp(current->args[0], "export") == 0 || ft_strcmp(current->args[0], "unset") == 0 || ft_strcmp(current->args[0], "cd") == 0) && current->next == NULL))
		data->last_exit_status = execute_builtin(current, data, false);
	while (current != NULL)
	{
		t_token *input_token = NULL;
		t_token *output_token = NULL;
		if (current->input)
		{
			input_token = find_token_by_value(data->tokens, current->input);
		}
		if (current->output)
		{
			output_token = find_token_by_value(data->tokens, current->output);
		}
		if (input_token && output_token)
		{
			if (input_token->word < output_token->word)
			{
				if (access(current->input, F_OK) != 0)
				{
					ft_fprintf(2, ": %s: No such file or directory\n", current->input);
					data->last_exit_status = 1;
					current = current->next;
					continue;
				}
			}
			else
			{
				output_dir = get_directory_from_path(current->output);
				if (output_dir && access(output_dir, F_OK) != 0)
				{
					ft_fprintf(2, ": %s: No such file or directory\n", current->output);
					data->last_exit_status = 1;
					free(output_dir);
					current = current->next;
					continue;
				}
				free(output_dir);
			}
		}
		else if (input_token)
		{
			if (access(current->input, F_OK) != 0)
			{
				ft_fprintf(2, ": %s: No such file or directory\n", current->input);
				data->last_exit_status = 1;
				current = current->next;
				continue;
			}
		}
		else if (output_token)
		{
			output_dir = get_directory_from_path(current->output);
			if (output_dir && access(output_dir, F_OK) != 0)
			{
				ft_fprintf(2, ": %s: No such file or directory\n", current->output);
				data->last_exit_status = 1;
				free(output_dir);
				current = current->next;
				continue;
			}
			free(output_dir);
		}
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
			else if (current->output != NULL)
			{
				if (current->append_output)
					fd_out = open(current->output, O_WRONLY | O_CREAT | O_APPEND, 0644);
				else
					fd_out = open(current->output, O_WRONLY | O_CREAT | O_TRUNC, 0644);
				if (fd_out < 0)
				{
					ft_fprintf(2, ": %s: ", current->output);
					perror("");
					cleanup_data(data, true);
					free(child_pids);
					exit(EXIT_FAILURE);
				}
				dup2(fd_out, STDOUT_FILENO);
				close(fd_out);
			}
			if (is_builtin(current->args[0]))
			{
				if (ft_strcmp(current->args[0], "exit") == 0)
				{
					cleanup_data(data, true);
					free(child_pids);
					exit(0);
				}
				if (!(ft_strcmp(current->args[0], "export") == 0) && !(ft_strcmp(current->args[0], "unset") == 0) && !(ft_strcmp(current->args[0], "cd") == 0))
				{
					data->last_exit_status = execute_builtin(current, data, false);
				}
				cleanup_data(data, true);
				free(child_pids);
				exit(data->last_exit_status);
			}
			else
			{
				data->last_exit_status = execute_single_cmd(current, data);
				cleanup_data(data, true);
				free(child_pids);
				exit(data->last_exit_status);
			}
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
	return (data->last_exit_status);
}

void print_parsed_commands(t_cmd *cmd_list)
{
	t_cmd *current_cmd;
	int i;

	current_cmd = cmd_list;
	while (current_cmd)
	{
		printf("Command:\n");
		for (i = 0; current_cmd->args[i]; i++)
			printf("  Arg[%d]: %s\n", i, current_cmd->args[i]);
		if (current_cmd->input)
			printf("  Input: %s\n", current_cmd->input);
		if (current_cmd->output)
			printf("  Output: %s\n", current_cmd->output);
		if (current_cmd->is_heredoc)
			printf("  Heredoc Delim: %s\n", current_cmd->heredoc_delim);
		current_cmd = current_cmd->next;
	}
}

int	main(int argc, char **argv, char **env_vars)
{
	t_data	data;

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
			// Check if the last character is a newline and remove it
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
			cleanup_data(&data, true);
			continue;
		}
		if (lexer(data.input, &data, data.last_exit_status) == -1)
		{
			cleanup_data(&data, true);
			continue ;
		}
		if (check_commands_in_tokens(data.tokens, &data) == -1)
		{
			cleanup_data(&data, false);
			continue ;
		}
		data.cmd_list = parse_tokens(&data);
		if (!data.cmd_list)
		{
			cleanup_data(&data, true);
			continue ;
		}
		data.last_exit_status = execute_cmd_list(&data);
		if (data.exit_flag == true)
			return (cleanup_data(&data, true), data.last_exit_status);
	}
	return (cleanup_data(&data, true), 0);
}
