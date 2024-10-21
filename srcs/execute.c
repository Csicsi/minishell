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
	while (cmd->args[i] && *(cmd->args[i]) == '\0')
		i++;
	if (cmd->args[i] == NULL || *(cmd->args[i]) == '\0')
	{
		cleanup_data(data, true);
		return (127);
	}
	if (is_builtin(cmd->args[i]))
		return (execute_builtin(cmd, data, false));
	cmd_path = find_cmd_path(cmd->args + i, data);
	if (!cmd_path)
	{
		ft_fprintf(2, ": %s: command not found\n", cmd->args[i]);
		cleanup_data(data, true);
		return (127);
	}
	execve(cmd_path, cmd->args + i, data->env_vars);
	if (!(*(cmd->args[i]) == '\0'))
		perror("minishell");
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

	num_children = 0;
	num_commands = count_cmds(data->cmd_list);
	child_pids = malloc(sizeof(pid_t) * num_commands);
	if (!child_pids)
	{
		perror("minishell: malloc");
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
			perror("minishell: fork");
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

/*
t_cmd	*parse_tokens(t_data *data)
{
	t_cmd	*cmd;
	t_cmd	*current_cmd;
	int			arg_index;
	int			words_count;
	t_token		*tmp;

	cmd = malloc(sizeof(t_cmd));
	if (!cmd)
		return (NULL);
	current_cmd = cmd;
	arg_index = 0;
	words_count = count_words(data->tokens);
	current_cmd->args = ft_calloc(words_count + 1, sizeof(char *));
	if (!current_cmd->args)
		return (NULL);
	current_cmd->input = NULL;
	current_cmd->output = NULL;
	current_cmd->append_output = 0;
	current_cmd->exit_status = 0;
	current_cmd->heredoc_delim = NULL;
	current_cmd->is_heredoc = false;
	current_cmd->next = NULL;
	tmp = data->tokens;
	while (data->tokens)
	{
		if (data->tokens->type == TOKEN_WORD)
			current_cmd->args[arg_index++] = ft_strdup(data->tokens->value);
		else if (data->tokens->type == TOKEN_OPERATOR)
		{
			if (ft_strcmp(data->tokens->value, "<<") == 0)
			{
				current_cmd->is_heredoc = true;
				if (data->tokens->next != NULL)
				{
					current_cmd->heredoc_delim = ft_strdup(data->tokens->next->value);
					data->tokens = data->tokens->next;
				}
				else
				{
					ft_fprintf(2, "minishell: syntax error near unexpected token newline\n");
					data->last_exit_status = 2;
					return (NULL);
				}
			}
			else if (ft_strcmp(data->tokens->value, ">") == 0)
			{
				if (data->tokens->next != NULL)
				{
					current_cmd->output = ft_strdup(data->tokens->next->value);
					data->tokens = data->tokens->next;
					if (data->tokens->next != NULL)
					{
						if ((arg_index == 0) && !current_cmd->args[0])
						{
							current_cmd->args[arg_index++] = ft_strdup(data->tokens->next->value);
							data->tokens = data->tokens->next;
						}
					}
					else
					{
						if ((arg_index == 0) && !current_cmd->args[0])
							current_cmd->args[arg_index++] = ft_strdup("");
					}
				}
				else
				{
					ft_fprintf(2, "minishell: syntax error near unexpected token newline\n");
					data->last_exit_status = 2;
					return (NULL);
				}
			}
			else if (ft_strcmp(data->tokens->value, ">>") == 0)
			{
				if (data->tokens->next != NULL)
				{
					current_cmd->output = ft_strdup(data->tokens->next->value);
					current_cmd->append_output = 1;
					data->tokens = data->tokens->next;
				}
				else
				{
					ft_fprintf(2, "minishell: syntax error near unexpected token newline\n");
					data->last_exit_status = 2;
					return (NULL);
				}
			}
			else if (ft_strcmp(data->tokens->value, "<") == 0)
			{
				if (data->tokens->next != NULL)
				{
					if (access(data->tokens->next->value, F_OK) == 0)
					{
						current_cmd->input = ft_strdup(data->tokens->next->value);
						data->tokens = data->tokens->next;
						if (data->tokens->next == NULL || data->tokens->next->type == TOKEN_OPERATOR)
						{
							if (data->tokens->next)
								ft_fprintf(2, "%s: command not found\n", data->tokens->next->value);
							else
								ft_fprintf(2, "command: command not found\n");
							return (NULL);
						}
					}
					else
					{
						ft_fprintf(2, ": %s: No such file or directory\n", data->tokens->next->value);
						return (NULL);
					}
					while (data->tokens && ft_strcmp(data->tokens->value, "|") != 0)
						data->tokens = data->tokens->next;
				}
				else
				{
					ft_fprintf(2, ": syntax error near unexpected token newline\n");
					data->last_exit_status = 2;
					return (NULL);
				}
			}
			else if (ft_strcmp(data->tokens->value, "|") == 0)
			{
				current_cmd->args[arg_index] = NULL;
				current_cmd->next = malloc(sizeof(t_cmd));
				if (!current_cmd->next)
					return (NULL);
				current_cmd = current_cmd->next;
				arg_index = 0;
				words_count = count_words(data->tokens);
				current_cmd->args = malloc(sizeof(char *) * (words_count + 1));
				if (!current_cmd->args)
					return (NULL);
				current_cmd->input = NULL;
				current_cmd->output = NULL;
				current_cmd->append_output = 0;
				current_cmd->exit_status = 0;
				current_cmd->heredoc_delim = NULL;
				current_cmd->is_heredoc = false;
				current_cmd->next = NULL;
				arg_index = 0;
			}
		}
		if (data->tokens)
			data->tokens = data->tokens->next;
	}
	current_cmd->args[arg_index] = NULL;
	data->tokens = tmp;
	free_tokens(data);
	return (cmd);
}
*/

int	main(int argc, char **argv, char **env_vars)
{
	t_data	data;

 	if (initialize(&data, env_vars, argc, argv))
		return (1);
	while (1)
	{
		if (get_input_line(&data) == NULL)
			return (cleanup_data(&data, true), data.last_exit_status);
		if (lexer(data.input, &data, data.last_exit_status) == -1
			|| check_commands_in_tokens(data.tokens, &data) == -1)
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
