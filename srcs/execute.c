#include "../includes/minishell.h"

/**
 * @brief Checks if a command is a recognized builtin.
 *
 * This function compares the command name with known builtins.
 *
 * @param command_name The name of the command.
 * @return int 1 if the command is a builtin, 0 if it is not.
 */
int	is_builtin(char *command_name)
{
	if (!command_name)  // Check if the command name is NULL
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

/**
 * @brief Executes the appropriate builtin command based on the command name.
 *
 * This function handles the execution of builtins like cd, echo, and exit.
 *
 * @param cmd The command structure containing arguments.
 * @return int The exit status of the executed builtin command.
 */
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
	return (1); // Return 1 if the command is not recognized as a builtin
}

/**
 * @brief Finds the appropriate path to run executables from the environment variable $PATH (typically in /usr/bin)
 *
 * @param cmd_args The command structure containing arguments.
 * @return char * The path where the cmd (executable) was found
 */
char	*find_cmd_path(char **cmd_args, t_data *data)
{
	int		i;
	char	**allpath;
	char	*path_env;
	char	*path_for_execve;

	// First, check if the input command is already an absolute path
	if (access(cmd_args[0], F_OK | X_OK) == 0)
		return (ft_strdup(cmd_args[0]));  // Return a duplicate of the command path

	// If not an absolute path, look for the command in the PATH environment variable
	path_env = ft_getenv(strdup("PATH"), data->env_vars);

	if (!path_env)
		return (NULL);  // In case PATH is not found, return NULL

	allpath = ft_split(path_env, ':');
	i = -1;
	while (allpath[++i])
	{
		path_for_execve = ft_strjoin_pipex(allpath[i], cmd_args[0]);

		if (access(path_for_execve, F_OK | X_OK) == 0)
			return (free_array_of_strs(allpath), path_for_execve);
		path_for_execve = free_null(path_for_execve);
	}
	return (free_array_of_strs(allpath), NULL);
}

int handle_heredoc(t_command *cmd)
{
    int pipe_fd[2];
    char *line;

    if (pipe(pipe_fd) == -1)
    {
        perror("pipe");
        return (-1);
    }

    // Keep reading input until the heredoc delimiter is encountered
    while (true)
    {
        line = readline("> ");  // Prompt the user for input
        if (!line || strcmp(line, cmd->heredoc_delim) == 0)
        {
            free(line);
            break;  // Stop when the delimiter is found or input is NULL (Ctrl+D)
        }

        // Write the input line to the pipe
        write(pipe_fd[1], line, ft_strlen(line));
        write(pipe_fd[1], "\n", 1);  // Add a newline after each input line
        free(line);
    }

    close(pipe_fd[1]);  // Close the write end of the pipe
    return pipe_fd[0];  // Return the read end of the pipe, which will be used as input
}

/**
 * @brief Executes a single command, handling input/output redirection and builtins.
 *
 * This function forks the process to run external commands and manages redirection.
 *
 * @param cmd The command structure containing arguments.
 * @param tokens Array of tokens to free after execution.
 * @param token_count Total number of tokens.
 * @return int The exit status of the executed command.
 */
int execute_single_cmd(t_command *cmd, t_data *data)
{
    int fd_in = -1;
    int fd_out = -1;
    char *cmd_path;
    int i = 0;  // Add index for argument checking

    // Handle input redirection if specified
    if (cmd->input != NULL)
    {
        fd_in = open(cmd->input, O_RDONLY);  // Open input file
        if (fd_in < 0)  // Error check
        {
            fprintf(stderr, ": %s: ", cmd->input);
            perror("");
            return (1);
        }
        dup2(fd_in, STDIN_FILENO);  // Redirect standard input
        close(fd_in);  // Close the original file descriptor
    }

    // Handle heredoc input redirection
    if (cmd->is_heredoc)
    {
        fd_in = handle_heredoc(cmd);  // Process the heredoc input
        if (fd_in == -1)
        {
            return (1);  // Error handling
        }
        dup2(fd_in, STDIN_FILENO);  // Redirect heredoc input to stdin
        close(fd_in);  // Close the file descriptor after dup2
    }

    // Handle output redirection if specified
    if (cmd->output != NULL)
    {
        fd_out = open(cmd->output, O_WRONLY | O_CREAT | (cmd->append_output ? O_APPEND : O_TRUNC), 0644);  // Open output file
        if (fd_out < 0)  // Error check
        {
            fprintf(stderr, ": %s: ", cmd->output);
            perror("");
            return (1);
        }
        dup2(fd_out, STDOUT_FILENO);  // Redirect standard output
        close(fd_out);  // Close the original file descriptor
    }

    // Find the first non-empty argument
    while (cmd->args[i] && *(cmd->args[i]) == '\0')
        i++;

    // If no non-empty argument was found, print ": : command not found"
    if (cmd->args[i] == NULL || *(cmd->args[i]) == '\0')
    {
        fprintf(stderr, ": : command not found\n");
        cleanup_data(data, true);
        return (127);  // Exit with 127 to indicate "command not found"
    }

    // Check if the command is a builtin and execute it
    if (is_builtin(cmd->args[i]))
        return (execute_builtin(cmd, data, false));

    // Find the command path using environment PATH or check if it's an absolute path
    cmd_path = find_cmd_path(cmd->args + i, data);  // Use the correct argument for path finding
    if (!cmd_path)
    {
        // If the command is not found, print an error and return
        fprintf(stderr, ": %s: command not found\n", cmd->args[i]);
        cleanup_data(data, true);
        return (127);  // Exit with 127 to indicate "command not found"
    }

    // Try to execute the command
    execve(cmd_path, cmd->args + i, data->env_vars);

    // If execve fails, print an error and clean up
    if (!(*(cmd->args[i]) == '\0'))  // if something (e.g. $notexists evaluates to "") do not print perror
        perror("minishell");

    free(cmd_path);  // Free the command path
    cleanup_data(data, true);
    exit(EXIT_FAILURE);  // Exit with a failure status
}

int count_commands(t_command *cmd_list)
{
    int count = 0;
    while (cmd_list)
    {
        count++;
        cmd_list = cmd_list->next;
    }
    return count;
}

/**
 * @brief Executes a list of commands with optional piping and redirection.
 *
 * This function sets up pipes between commands and handles input/output redirection.
 *
 * @param cmd The first command in the linked list of commands.
 * @return int The exit status of the last command executed.
 */
int execute_cmd_list(t_data *data)
{
    t_command *current;
    int pipe_fd[2];
    int prev_fd;
    pid_t pid;
    int num_children = 0;  // Counter for the number of child processes

    int num_commands = count_commands(data->cmd_list);  // Count the number of commands
    pid_t *child_pids = malloc(sizeof(pid_t) * num_commands);  // Dynamically allocate memory for child PIDs
    if (!child_pids)
    {
        perror("minishell: malloc");
        return (1);  // Exit on memory allocation failure
    }

    current = data->cmd_list;
    prev_fd = -1;

    if (current->args[0] && strcmp(current->args[0], "exit") == 0 && current->next == NULL)
    {
        data->last_exit_status = execute_builtin(current, data, true);  // Execute exit command directly
        cleanup_data(data, true);
        free(child_pids);  // Free allocated memory for child_pids
        return (data->last_exit_status);  // Return exit status directly
    }

    // Execute built-in functions like export, unset (and cd?) directly in the parent process
    if (current->args[0] != NULL && ((strcmp(current->args[0], "export") == 0 || strcmp(current->args[0], "unset") == 0
        || strcmp(current->args[0], "cd") == 0) && current->next == NULL))
    {
        data->last_exit_status = execute_builtin(current, data, false);
    }

    while (current != NULL)  // Loop through each command in the list
    {
        if (current->next != NULL)
            pipe(pipe_fd);  // Create a pipe if there's a next command

        pid = fork();  // Fork the process for the current command
        if (pid == 0)  // In child process
        {
            if (prev_fd != -1)  // If there's a previous command, set up input redirection
            {
                dup2(prev_fd, STDIN_FILENO);  // Redirect input to previous pipe
                close(prev_fd);  // Close read end of the previous pipe in the child
            }
            if (current->next != NULL)  // If there's a next command, set up output redirection
            {
                dup2(pipe_fd[1], STDOUT_FILENO);  // Redirect output to next pipe
                close(pipe_fd[0]);  // Close unused read end of the pipe in the child
                close(pipe_fd[1]);  // Close write end after dup2
            }
            else if (current->output != NULL)  // Handle output redirection for the last command
            {
                int fd_out = open(current->output, O_WRONLY | O_CREAT | (current->append_output ? O_APPEND : O_TRUNC), 0644);
                if (fd_out < 0)  // Check for errors
                {
                    fprintf(stderr, ": %s: ", current->output);
                    perror("");
                    cleanup_data(data, true);
                    free(child_pids);
                    exit(EXIT_FAILURE);
                }
                dup2(fd_out, STDOUT_FILENO);  // Redirect standard output
                close(fd_out);  // Close file descriptor after redirection
            }

            // Execute the command (built-in or external)
            if (is_builtin(current->args[0]))  // If built-in, execute it in the child
            {
                if (strcmp(current->args[0], "exit") == 0)
                {
                    cleanup_data(data, true);
                    free(child_pids);
                    exit(0);
                }

                if (!(strcmp(current->args[0], "export") == 0) && !(strcmp(current->args[0], "unset") == 0)
                    && !(strcmp(current->args[0], "cd") == 0))
                {
                    data->last_exit_status = execute_builtin(current, data, false);
                }
                cleanup_data(data, true);
                free(child_pids);
                exit(data->last_exit_status);  // Exit with the status of the built-in
            }
            else
            {
                data->last_exit_status = execute_single_cmd(current, data);
                cleanup_data(data, true);
                free(child_pids);
                exit(data->last_exit_status);
            }
        }
        else if (pid > 0)  // In parent process
        {
            child_pids[num_children++] = pid;  // Store the PID of the child process

            // Close the file descriptors in the parent after they are no longer needed
            if (prev_fd != -1)
            {
                close(prev_fd);  // Close previous pipe read end in the parent
            }

            if (current->next != NULL)
            {
                close(pipe_fd[1]);  // Close current pipe write end in the parent
                prev_fd = pipe_fd[0];  // Keep read end for the next iteration
            }
            else
            {
                prev_fd = -1;  // No more pipes after this
            }
        }
        else
        {
            perror("minishell: fork");  // If fork fails, print error
            cleanup_data(data, true);
            free(child_pids);  // Free dynamically allocated child_pids array
            return (1);
        }

        current = current->next;  // Move to the next command in the list
    }

    // Wait for all child processes to finish
    for (int i = 0; i < num_children; i++)
    {
        waitpid(child_pids[i], &data->last_exit_status, 0);  // Wait for each child
        if (WIFEXITED(data->last_exit_status))
            data->last_exit_status = WEXITSTATUS(data->last_exit_status);  // Get the exit status of the last child process
    }

    cleanup_data(data, false);
    free(child_pids);  // Free dynamically allocated memory for child_pids
    return (data->last_exit_status);  // Return the exit status of the last command executed
}

int count_words(t_token *tokens)
{
    int word_count = 0;

    while (tokens)
    {
		if ((strcmp(tokens->value, "|") == 0) && tokens->next != NULL)
		{
			tokens = tokens->next;
		}
        if (tokens->type == TOKEN_WORD)
        {
            word_count++;
        }
        else if (tokens->type == TOKEN_OPERATOR)
        {
            if (strcmp(tokens->value, "|") == 0)
                break;  // Stop counting at the pipe operator
        }
        tokens = tokens->next;  // Move to the next token
    }

    return word_count;
}


/**
 * @brief Parses tokens into a linked list of command structures.
 *
 * This function processes an array of tokens generated by the lexer and
 * organizes them into a linked list of t_command structures. Each command
 * includes its name, arguments, input/output redirection, and other necessary
 * execution parameters.
 *
 * @param tokens Array of tokens to be parsed.
 * @param token_count The total number of tokens to parse.
 * @return t_command* The head of the linked list of commands.
 */
t_command *parse_tokens(t_data *data)
{
    t_command *cmd = malloc(sizeof(t_command));  // Allocate memory for the first command structure
    if (!cmd)
    {
        return (NULL);
    }

    t_command *current_cmd = cmd;  // Set the current command pointer to the first command
    int arg_index = 0;

    int words_count = count_words(data->tokens);

    // Initialize the first command's fields to default values
    current_cmd->args = calloc(words_count + 1, sizeof(char *));  // Use calloc to initialize to NULL
    if (!current_cmd->args)
    {
        return NULL;
    }

    current_cmd->input = NULL;
    current_cmd->output = NULL;
    current_cmd->append_output = 0;
    current_cmd->exit_status = 0;
    current_cmd->heredoc_delim = NULL;
    current_cmd->is_heredoc = false;
    current_cmd->next = NULL;

    t_token *tmp = data->tokens;  // Keep a pointer to the head of the token list for cleanup

    // Iterate through the token linked list
    while (data->tokens)
    {
        // Handle command words (non-operator tokens)
        if (data->tokens->type == TOKEN_WORD)
        {
            current_cmd->args[arg_index++] = strdup(data->tokens->value);  // First word and subsequent words are arguments
        }
        // Handle operators (such as pipes and redirection)
        else if (data->tokens->type == TOKEN_OPERATOR)
        {
            // Handle heredoc operator: "<<"
            if (strcmp(data->tokens->value, "<<") == 0)
            {
                current_cmd->is_heredoc = true;  // Set heredoc flag to true
                if (data->tokens->next != NULL)
                {
                    current_cmd->heredoc_delim = strdup(data->tokens->next->value);  // Store heredoc delimiter
                    data->tokens = data->tokens->next;  // Move to the next token
                }
                else
                {
                    fprintf(stderr, "minishell: syntax error near unexpected token newline\n");
                    return NULL;
                }
            }
            // Handle output redirection (">")
			else if (strcmp(data->tokens->value, ">") == 0)
			{
    			// Check if there's a token after the redirection, which would be the output file
    			if (data->tokens->next != NULL)
    			{
        			// Set the output file name from the next token
        			current_cmd->output = strdup(data->tokens->next->value);
        			data->tokens = data->tokens->next;  // Move to the token for the output file

        			// Check if there's another token after the output file, which would be the command/argument
        			if (data->tokens->next != NULL)
        			{
            			// Copy the next token (command or argument) into arg[0]
            			if ((arg_index == 0) && !current_cmd->args[0])
            			{
                			current_cmd->args[arg_index++] = strdup(data->tokens->next->value);
                			data->tokens = data->tokens->next;  // Move to the command/argument token
            			}
					}
					else
					{
						// If there's no command after the output file, set arg[0] to an empty string
						if ((arg_index == 0) && !current_cmd->args[0])
							current_cmd->args[arg_index++] = strdup("");
					}
				}
				else
				{
					// Handle syntax error when no output file is provided
					fprintf(stderr, "minishell: syntax error near unexpected token newline\n");
					return NULL;
				}
			}
            // Handle append output redirection (">>")
            else if (strcmp(data->tokens->value, ">>") == 0)
            {
                if (data->tokens->next != NULL)
                {
                    current_cmd->output = strdup(data->tokens->next->value);  // Set the output file name
                    current_cmd->append_output = 1;  // Set append flag
                    data->tokens = data->tokens->next;
                }
                else
                {
                    fprintf(stderr, "minishell: syntax error near unexpected token newline\n");
                    return NULL;  // Return NULL or handle the error appropriately
                }
            }
            // Handle input redirection ("<")
            else if (strcmp(data->tokens->value, "<") == 0)  // Handle input redirection "<"
			{
				if (data->tokens->next != NULL)
				{
					// Check if the input file exists
					if (access(data->tokens->next->value, F_OK) == 0)
					{
						current_cmd->input = strdup(data->tokens->next->value);   // Set the input file name from the next token
						data->tokens = data->tokens->next;

						// Check if the token after input redirection is a valid command
						if (data->tokens->next == NULL || data->tokens->next->type == TOKEN_OPERATOR)
						{
							// If there is no command or the next token is an operator, print an error message
							fprintf(stderr, "%s: command not found\n", data->tokens->next ? data->tokens->next->value : "command");
							return NULL;
						}
					}
					else
					{
						// Handle the case where the input file doesn't exist
						fprintf(stderr, ": %s: No such file or directory\n", data->tokens->next->value);
						return NULL;
					}

					// Skip all tokens until the next command (pipe or end)
					while (data->tokens && strcmp(data->tokens->value, "|") != 0)
					{
						data->tokens = data->tokens->next;
					}
				}
				else
				{
					fprintf(stderr, ": syntax error near unexpected token newline\n");
					return NULL;  // Handle the case where input redirection has no file provided
				}
			}

            // Handle pipe operator: create a new command
            else if (strcmp(data->tokens->value, "|") == 0)
            {
                current_cmd->args[arg_index] = NULL;  // Null-terminate the current command's arguments
                // Allocate memory for the next command in the pipeline
                current_cmd->next = malloc(sizeof(t_command));
                if (!current_cmd->next)
                    return NULL;

                current_cmd = current_cmd->next;  // Move to the next command
				arg_index = 0;  // Reset argument index for the new command
				// Skip the pipe token and move to the next token before counting words for the new command
                words_count = count_words(data->tokens);

                // Initialize the new command's fields
                current_cmd->args = malloc(sizeof(char *) * (words_count + 1));
                if (!current_cmd->args)
                {
                    return NULL;
                }

                current_cmd->input = NULL;
                current_cmd->output = NULL;
                current_cmd->append_output = 0;
                current_cmd->exit_status = 0;
                current_cmd->heredoc_delim = NULL;
                current_cmd->is_heredoc = false;
                current_cmd->next = NULL;
                arg_index = 0;  // Reset argument index for the new command
            }
        }
        if (data->tokens)  // Ensure data->tokens is not NULL
		{
			data->tokens = data->tokens->next;  // Move to the next token
		}
    }

    current_cmd->args[arg_index] = NULL;  // Null-terminate the argument list for the last command

    // Free the token list after parsing since it's no longer needed
    data->tokens = tmp;  // Restore pointer to the head of the list
    free_tokens(data);    // Free all tokens

    return cmd;  // Return the head of the command list
}

/**
 * @brief Checks if (single or double) quotes in input string remain open
 * This function checks if there are (single or double) quotes opening up from
 * readline but don't close. If they remain open, return in_quote = 1
 *
 * @param cursor The input string.
 * @return a flag that is in_quote = 1 if quotes remain open, in_quote = 0 if quotes are closed
 */
int	check_for_unclosed_quotes(char *cursor)
{
	int		in_quote = 0;
	char	quote_char = '\0';

	while (*cursor)
	{
		// Handle quotes in the first pass
		if ((*cursor == '"' || *cursor == '\'') && in_quote == 0)
		{
			in_quote = 1;
			quote_char = *cursor;
		}
		else if (*cursor == quote_char && in_quote == 1)
		{
			in_quote = 0;  // Quote closed
		}
		cursor++;
	}
	return (in_quote);
}

/**
 * @brief Creates a copy of the env_vars (entered as 3rd argument in main) on dynamic memory
 *
 * The malloc-ed copy of environment variables passed as a third argument to the main is useful
 * for builtin functions export and unset, which add/remove env vars. Clearer coding practise to have
 * everything on dynamic memory, rather than having to keep track of what is on stack and what is
 * dynamic/ needs to be freed
 *
 * @return char **Copy of env_vars on dynamic memory
 */
char **duplicate_env_vars(char **env_vars)
{
    int i = 0;
    char **new_env_vars;

    // Count how many environment variables there are
    while (env_vars[i] != NULL)
        i++;

    // Allocate memory for the array of environment variable pointers
    new_env_vars = (char **)malloc((i + 1) * sizeof(char *));  // +1 for the NULL terminator
    if (!new_env_vars)
        return (NULL);  // Return NULL if memory allocation fails

    // Copy each environment variable to dynamic memory
    i = 0;
	while (env_vars[i] != NULL)
    {
        // Allocate memory for each string and copy the content
        new_env_vars[i] = strdup(env_vars[i]);
        if (!new_env_vars[i])
        {
            // Free already allocated memory in case of failure
            while (i-- > 0)
                free(new_env_vars[i]);
            free(new_env_vars);
            return (NULL);
        }
		i++;
    }
    // Set the last element to NULL (to indicate the end of the env_vars array)
    new_env_vars[i] = NULL;
    return (new_env_vars);
}

void handle_sigint(int sig)
{
    (void)sig;
    // Print a newline to move to the next line
    write(STDOUT_FILENO, "\n", 1);
    // Reissue the prompt on a new line
    rl_on_new_line();
    rl_replace_line("", 0);
    rl_redisplay();
}

/**
 * @brief Prints the tokens after lexing for debugging purposes.
 *
 * This function goes through the array of tokens and prints their types and values
 * for each token, allowing you to inspect the output of the lexer.
 *
 * @param tokens The array of tokens.
 */
void print_tokens(t_token *tokens)
{
    int i = 0;

    printf("Tokens after lexing:\n");
    while (tokens)
    {
        printf("Token %d: Type = ", i);
        switch (tokens->type)
        {
            case TOKEN_WORD:
                printf("WORD, ");
                break;
            case TOKEN_OPERATOR:
                printf("OPERATOR, ");
                break;
            default:
                printf("UNKNOWN, ");
                break;
        }

        printf("Value = '%s', Word = %d\n", tokens->value ? tokens->value : "(null)", tokens->word);

        tokens = tokens->next;  // Move to the next token in the list
        i++;
    }
}

/**
 * @brief Checks if the first token and the token after a pipe are valid commands.
 *
 * This function loops through the tokens and ensures that the first token and
 * any token immediately following a pipe ('|') is a word (command).
 * If any of these tokens is not a command, it returns an error.
 *
 * @param tokens The array of tokens to check.
 * @return int Returns 0 if all checks pass, otherwise -1 for an error.
 */
int check_commands_in_tokens(t_token *tokens)
{
    t_token *current = tokens;  // Start with the head of the list

    // Special case: If the first token is neither a word nor a valid operator, return an error
    if (!current || (current->type != TOKEN_WORD && current->type != TOKEN_OPERATOR))
    {
        return (-1);
    }

    // Loop through the rest of the tokens
    while (current)
    {
        // If a pipe is found, the next token must be a command or a redirection operator
        if (current->type == TOKEN_OPERATOR && strcmp(current->value, "|") == 0)
        {
            current = current->next;  // Move to the next token after the pipe

            // If the token after the pipe is missing or invalid, return an error
            if (!current || (current->type != TOKEN_WORD && (strcmp(current->value, ">") != 0 && strcmp(current->value, "<") != 0)))
            {
                fprintf(stderr, ": syntax error near unexpected token `%s'\n", current ? current->value : "newline");
                return (-1);
            }
        }
        // Handle output/input redirection operators (>, >>, <)
        else if (current->type == TOKEN_OPERATOR && (strcmp(current->value, ">") == 0 || strcmp(current->value, ">>") == 0 || strcmp(current->value, "<") == 0))
        {
            current = current->next;  // Move to the next token after the redirection
            if (!current || current->type != TOKEN_WORD)
            {
                fprintf(stderr, ": syntax error near unexpected token `%s'\n", current ? current->value : "newline");
                return (-1);
            }
        }
        current = current->next;  // Move to the next token
    }

    return 0;  // All checks passed
}

/**
 * @brief Entry point for the minishell program.
 *
 * The main function is responsible for running the main input loop of the shell,
 * reading user input, tokenizing it, parsing it into commands, and executing those
 * commands.
 *
 * @return int Exit status of the program (0 for success, 1 for errors).
 */
int	main(int argc, char **argv, char **env_vars)
{
	char		*input;         // Input string entered by the user
	/*char		*temp_input;
	char		*temp = NULL;
	char		*full_input = NULL;*/
	int			in_quote;
	t_data		data;

	(void)argc;
	(void)argv;

	// Set up signal handlers
    signal(SIGINT, handle_sigint);   // Handle Ctrl+C

	data.exit_flag = false;
	data.last_exit_status = 0;
	data.token_count = 0;
	data.input = NULL;
	data.env_vars = NULL;
	data.tokens = NULL;
	data.cmd_list = NULL;

	// Duplicate env_vars into dynamic memory
	data.env_vars = duplicate_env_vars(env_vars);
	if (!data.env_vars)
	{
		printf("Error allocating memory for env_vars");
		return (1); // Return 1 to indicate an error occurred
	}

	// Infinite loop to keep the shell running until "exit" is entered
	while (1)
	{
		// Prompt the user and read input using readline
		input = readline("Don'tPanicShell> ");

		// If the user inputs Ctrl-D (EOF), exit the shell
		if (input == NULL)
		{
			//fprintf(stderr, "exit\n");
			break;
		}

		if (*input)  // Only add non-empty input to history
			add_history(input);

		in_quote = check_for_unclosed_quotes(input);
		if (in_quote == 1)
		{
			fprintf(stderr, "syntax error: unclosed quote\n");
			cleanup_data(&data, true);
			return (1);  // Return 1 to indicate an error occurred
		}

		/*while (in_quote == 1)
		{
			temp_input = readline("> "); // Show a different prompt for continued input
			if (temp_input)
			{
				// Concatenate input with previous input
				temp = ft_strjoin(input, "\n");
				free(input);
				full_input = ft_strjoin(temp, temp_input);
				free(temp);
				free(temp_input);
				input = full_input;
			}
			in_quote = check_for_unclosed_quotes(input);
		}*/

		// Tokenize the input string and check for errors during lexing
		//if (lexer(input, &data.tokens, data.token_count, data.last_exit_status) == -1)
		if (lexer(input, &data, data.last_exit_status) == -1)
		{
			cleanup_data(&data, true);
			continue ;
		}

		//print_tokens(data.tokens);

		if (check_commands_in_tokens(data.tokens) == -1)
		{
			cleanup_data(&data, false);
			continue ;
		}

		// Parse the tokens into a linked list of commands
		data.cmd_list = parse_tokens(&data);

		// If parsing fails, print an error message and clean up
		if (!data.cmd_list)
		{
			cleanup_data(&data, true);
			continue ;
		}

		// Execute the parsed list of commands and store the exit status of the last command
		data.last_exit_status = execute_cmd_list(&data);

		if (data.exit_flag == true)
		{
			cleanup_data(&data, true);
			return (data.last_exit_status);        // Return exit status
		}

	}

	// Final cleanup after the loop
	cleanup_data(&data, true);

	return (0); // Return 0 to indicate successful execution of the shell
}
