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

int	execute_builtin(t_command *cmd, t_data *data, bool print_exit)
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

char	*find_cmd_path(char **cmd_args, t_data *data)
{
	int		i;
	char	**allpath;
	char	*path_env;
	char	*path_for_execve;

	if (access(cmd_args[0], F_OK | X_OK) == 0)
		return (ft_strdup(cmd_args[0]));
	path_env = ft_getenv(ft_strdup("PATH"), data->env_vars);
	if (!path_env)
		return (NULL);
	allpath = ft_split(path_env, ':');
	i = -1;
	while (allpath[++i])
	{
		path_for_execve = ft_strjoin_pipex(allpath[i], cmd_args[0]);
		if (access(path_for_execve, F_OK | X_OK) == 0)
			return (free_string_array(allpath), path_for_execve);
		path_for_execve = free_null(path_for_execve);
	}
	return (free_string_array(allpath), NULL);
}

int	handle_heredoc(t_command *cmd)
{
	int		pipe_fd[2];
	char	*line;

	if (pipe(pipe_fd) == -1)
	{
		perror("pipe");
		return (-1);
	}
	while (true)
	{
		line = readline("> ");
		if (!line || ft_strcmp(line, cmd->heredoc_delim) == 0)
		{
			free(line);
			break ;
		}
		write(pipe_fd[1], line, ft_strlen(line));
		write(pipe_fd[1], "\n", 1);
		free(line);
	}
	close(pipe_fd[1]);
	return (pipe_fd[0]);
}

int	execute_single_cmd(t_command *cmd, t_data *data)
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
	if (cmd->args[i] == NULL || *(cmd->args[i]) == '\0')
	{
		ft_fprintf(2, ": %s: command not found\n", cmd->args[i]);
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

int	count_commands(t_command *cmd_list)
{
	int	count;

	count = 0;
	while (cmd_list)
	{
		count++;
		cmd_list = cmd_list->next;
	}
	return (count);
}

char	*get_directory_from_path(const char *path)
{
	int		i;
	char	*dir;

	// Find the last '/' in the path
	i = ft_strlen(path) - 1;
	while (i >= 0 && path[i] != '/')
		i--;

	// If no '/' was found, there is no directory in the path
	if (i < 0)
		return (NULL);

	// Allocate memory for the directory string and copy the directory portion
	dir = ft_strndup(path, i);
	if (!dir)
		return (NULL);

	return (dir);
}

t_token *find_token_by_value(t_token *tokens, const char *value)
{
    t_token *current = tokens;

    while (current != NULL)
    {
        if (ft_strcmp(current->value, value) == 0)
            return current;
        current = current->next;
    }

    return NULL;
}

int	execute_cmd_list(t_data *data)
{
	t_command	*current;
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
	num_commands = count_commands(data->cmd_list);
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
		// We now use the word positions to check which error to print first
		t_token *input_token = NULL;
		t_token *output_token = NULL;

		// Find the input and output tokens by position
		if (current->input)
		{
			input_token = find_token_by_value(data->tokens, current->input);
		}
		if (current->output)
		{
			output_token = find_token_by_value(data->tokens, current->output);
		}

		// If both input and output exist, check which comes first by comparing the word index
		if (input_token && output_token)
		{
			if (input_token->word < output_token->word)
			{
				// Input comes first, check if the file exists
				if (access(current->input, F_OK) != 0)
				{
					ft_fprintf(2, ": %s: No such file or directory\n", current->input);
					current = current->next;
					continue;  // Move to the next command if the input file is missing
				}
			}
			else
			{
				// Output comes first, check if the directory exists
				output_dir = get_directory_from_path(current->output);
				if (output_dir && access(output_dir, F_OK) != 0)
				{
					ft_fprintf(2, ": %s: No such directory\n", output_dir);
					free(output_dir);
					current = current->next;
					continue;  // Skip the command if the directory does not exist
				}
				free(output_dir);
			}
		}
		else if (input_token)
		{
			// Only input exists, check if the file exists
			if (access(current->input, F_OK) != 0)
			{
				ft_fprintf(2, ": %s: No such file or directory\n", current->input);
				current = current->next;
				continue;
			}
		}
		else if (output_token)
		{
			// Only output exists, check if the directory exists
			output_dir = get_directory_from_path(current->output);
			if (output_dir && access(output_dir, F_OK) != 0)
			{
				ft_fprintf(2, ": %s: No such directory\n", output_dir);
				free(output_dir);
				current = current->next;
				continue;
			}
			free(output_dir);
		}

		// Fork and execute the command
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

void print_parsed_commands(t_command *cmd_list)
{
	t_command *current_cmd;
	int i;

	current_cmd = cmd_list;
	while (current_cmd)
	{
		printf("Command:\n");
		for (i = 0; current_cmd->args[i]; i++)
		{
			printf("  Arg[%d]: %s\n", i, current_cmd->args[i]);
		}
		if (current_cmd->input)
			printf("  Input: %s\n", current_cmd->input);
		if (current_cmd->output)
			printf("  Output: %s\n", current_cmd->output);
		if (current_cmd->is_heredoc)
			printf("  Heredoc Delim: %s\n", current_cmd->heredoc_delim);
		current_cmd = current_cmd->next;
	}
}

t_command	*parse_tokens(t_data *data)
{
	t_command	*cmd;
	t_command	*current_cmd;
	int			arg_index;
	int			words_count;
	t_token		*tmp;

	cmd = malloc(sizeof(t_command));
	if (!cmd)
		return (NULL);
	current_cmd = cmd;
	arg_index = 0;
	words_count = count_tokens(data->tokens);
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
		{
			current_cmd->args[arg_index++] = ft_strdup(data->tokens->value);
		}
		else if (data->tokens->type == TOKEN_OPERATOR)
		{
			if (ft_strcmp(data->tokens->value, "<<") == 0)
			{
				// Check for heredoc and skip over the next token (heredoc delimiter)
				if (current_cmd->is_heredoc == false && data->tokens->next != NULL)
				{
					current_cmd->is_heredoc = true;
					current_cmd->heredoc_delim = ft_strdup(data->tokens->next->value);
					data->tokens = data->tokens->next;  // Skip over the delimiter
				}
				else
				{
					data->tokens = data->tokens->next;  // Skip operator and argument
				}
			}
			else if (ft_strcmp(data->tokens->value, ">") == 0)
			{
				// Skip if output is already set, otherwise set output and skip
				if (current_cmd->output == NULL && data->tokens->next != NULL)
				{
					current_cmd->output = ft_strdup(data->tokens->next->value);
					data->tokens = data->tokens->next;  // Skip over the output file
				}
				else
				{
					data->tokens = data->tokens->next;  // Skip operator and argument
				}
			}
			else if (ft_strcmp(data->tokens->value, ">>") == 0)
			{
				// Skip if output is already set, otherwise set output and skip
				if (current_cmd->output == NULL && data->tokens->next != NULL)
				{
					current_cmd->output = ft_strdup(data->tokens->next->value);
					current_cmd->append_output = 1;
					data->tokens = data->tokens->next;  // Skip over the output file
				}
				else
				{
					data->tokens = data->tokens->next;  // Skip operator and argument
				}
			}
			else if (ft_strcmp(data->tokens->value, "<") == 0)
			{
				// Skip if input is already set, otherwise set input and skip
				if (current_cmd->input == NULL && data->tokens->next != NULL)
				{
					current_cmd->input = ft_strdup(data->tokens->next->value);
					data->tokens = data->tokens->next;  // Skip over the input file
				}
				else
				{
					data->tokens = data->tokens->next;  // Skip operator and argument
				}
			}
			else if (ft_strcmp(data->tokens->value, "|") == 0)
			{
				// Start a new command when encountering a pipe
				current_cmd->args[arg_index] = NULL;
				current_cmd->next = malloc(sizeof(t_command));
				if (!current_cmd->next)
					return (NULL);
				current_cmd = current_cmd->next;
				arg_index = 0;
				words_count = count_tokens(data->tokens);
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
		{
			data->tokens = data->tokens->next;
		}
	}
	current_cmd->args[arg_index] = NULL;
	data->tokens = tmp;
	//print_parsed_commands(cmd);
	free_tokens(data);
	return (cmd);
}

int	check_for_unclosed_quotes(char *cursor)
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

void	handle_sigint(int sig)
{
	(void)sig;
	write(STDOUT_FILENO, "\n", 1);
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
}

int	check_commands_in_tokens(t_token *tokens, t_data *data)
{
	t_token	*current;

	current = tokens;
	if (!current || (current->type != TOKEN_WORD && current->type != TOKEN_OPERATOR))
		return (-1);
	while (current)
	{
		if (current->type == TOKEN_OPERATOR && ft_strcmp(current->value, "|") == 0)
		{
			current = current->next;
			if (!current || (current->type != TOKEN_WORD && (ft_strcmp(current->value, ">") != 0 && ft_strcmp(current->value, "<") != 0)))
			{
				if (current)
				{
					ft_fprintf(2, ": syntax error near unexpected token `%s'\n", current->value);
					data->last_exit_status = 2;
				}
				else
				{
					ft_fprintf(2, ": syntax error near unexpected token `newline'\n");
					data->last_exit_status = 2;
				}
				return (-1);
			}
		}
		else if (current->type == TOKEN_OPERATOR && (ft_strcmp(current->value, ">") == 0 || ft_strcmp(current->value, ">>") == 0 || ft_strcmp(current->value, "<") == 0))
		{
			current = current->next;
			if (!current || current->type != TOKEN_WORD)
			{
				if (current)
				{
					ft_fprintf(2, ": syntax error near unexpected token `%s'\n", current->value);
					data->last_exit_status = 2;
				}
				else
				{
					ft_fprintf(2, ": syntax error near unexpected token `newline'\n");
					data->last_exit_status = 2;
				}
				return (-1);
			}
		}
		current = current->next;
	}
	return (0);
}

int	main(int argc, char **argv, char **env_vars)
{
	int		in_quote;
	t_data	data;

	(void)argc;
	(void)argv;
	signal(SIGINT, handle_sigint);
	data.exit_flag = false;
	data.last_exit_status = 0;
	data.token_count = 0;
	data.input = NULL;
	data.env_vars = NULL;
	data.tokens = NULL;
	data.cmd_list = NULL;
	data.env_vars = duplicate_env_vars(env_vars);
	if (!data.env_vars)
	{
		printf("Error allocating memory for env_vars");
		return (1);
	}
	while (1)
	{
		data.input = readline("Don'tPanicShell> ");
		if (data.input == NULL)
		{
			cleanup_data(&data, true);
			return (data.last_exit_status);
		}
		if (*data.input)
			add_history(data.input);
		in_quote = check_for_unclosed_quotes(data.input);
		if (in_quote == 1)
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
		{
			cleanup_data(&data, true);
			return (data.last_exit_status);
		}
	}
	cleanup_data(&data, true);
	return (0);
}
