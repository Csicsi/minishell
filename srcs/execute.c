/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krabitsc <krabitsc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/14 14:31:51 by dcsicsak          #+#    #+#             */
/*   Updated: 2024/10/06 19:05:58 by krabitsc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/**
 * @brief Handles the `cd` builtin command.
 *
 * This function changes the current working directory.
 *
 * @param cmd The command structure containing arguments.
 * @return int Always returns 0 for success.
 */
int	builtin_cd(t_command *cmd)
{
	(void)cmd; // Ignore unused parameter
	printf("builtin_cd: Changing Directory...\n"); // Placeholder message
	return (0);
}

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
 * This function handles the execution of builtins like `cd`, `echo`, and `exit`.
 *
 * @param cmd The command structure containing arguments.
 * @return int The exit status of the executed builtin command.
 */
int	execute_builtin(t_command *cmd, t_data *data, bool print_exit)
{
	if (ft_strncmp(cmd->name, "cd", 3) == 0)
		return (builtin_cd(cmd));
	else if (ft_strncmp(cmd->name, "echo", 5) == 0)
		return (builtin_echo(cmd));
	else if (ft_strncmp(cmd->name, "exit", 5) == 0)
		return (builtin_exit(cmd, data, print_exit));
	else if (ft_strncmp(cmd->name, "env", 4) == 0)
		return (builtin_env(data));
	else if (ft_strncmp(cmd->name, "export", 7) == 0)
		return (builtin_export(cmd, data));
	else if (ft_strncmp(cmd->name, "unset", 6) == 0)
		return (builtin_unset(cmd, data));
	else if (ft_strncmp(cmd->name, "pwd", 4) == 0)
		return (builtin_pwd());
	return (1); // Return 1 if the command is not recognized as a builtin
}

/**
 * @brief Frees a linked list of commands.
 *
 * This function releases all allocated memory in the command list.
 *
 * @param cmd_list The head of the command linked list to free.
 */
void	free_cmd_list(t_command *cmd_list)
{
	t_command *tmp;

	while (cmd_list)
	{
		tmp = cmd_list;

		// Free command name if allocated
		if (cmd_list->name) {
			free(cmd_list->name);
			cmd_list->name = NULL;
		}

		// Free each argument in the args array if allocated
		if (cmd_list->args)
		{
			for (int i = 0; cmd_list->args[i]; i++)
			{
				free(cmd_list->args[i]);
				cmd_list->args[i] = NULL;
			}
			free(cmd_list->args); // Free the args array itself
			cmd_list->args = NULL;
		}

		// Free input redirection file name if allocated
		if (cmd_list->input) {
			free(cmd_list->input);
			cmd_list->input = NULL;
		}

		// Free output redirection file name if allocated
		if (cmd_list->output) {
			free(cmd_list->output);
			cmd_list->output = NULL;
		}

		// Move to the next command and free the current one
		cmd_list = cmd_list->next;
		free(tmp);
		tmp = NULL;
	}
}

/**
 * @brief Finds the appropriate path to run executables from the environment variable $PATH (typically in /usr/bin)
 *
 *
 * @param cmd_args The command structure containing arguments.
 * @return char * The path where the cmd (executable) was found
 */

char	*find_cmd_path(char **cmd_args)
{
	int		i;
	char	**allpath;
	char	*path_env;
	char	*path_for_execve;

	// First, check if the input command is already an absolute path
	if (access(cmd_args[0], F_OK | X_OK) == 0)
		return (ft_strdup(cmd_args[0]));  // Return a duplicate of the command path

	// If not an absolute path, look for the command in the PATH environment variable
	path_env = getenv("PATH");
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
int	execute_single_cmd(t_command *cmd, t_data *data)
{
	int		fd_in;
	int		fd_out;
	pid_t	pid;
	char	*cmd_path;

	// Handle input redirection if specified
	if (cmd->input != NULL)
	{
		fd_in = open(cmd->input, O_RDONLY); // Open input file
		if (fd_in < 0) // Check for errors
		{
			perror("minishell: input redirection");
			return (1);
		}
		dup2(fd_in, STDIN_FILENO); // Redirect standard input
		close(fd_in); // Close file descriptor
	}

	// Handle output redirection if specified
	if (cmd->output != NULL)
	{
		fd_out = open(cmd->output, O_WRONLY | O_CREAT | (cmd->append_output ? O_APPEND : O_TRUNC), 0644); // Open output file
		if (fd_out < 0) // Check for errors
		{
			perror("minishell: output redirection");
			return (1);
		}
		dup2(fd_out, STDOUT_FILENO); // Redirect standard output
		close(fd_out); // Close file descriptor
	}

	// Check if the command is a builtin and execute it
	if (is_builtin(cmd->name))
		return (execute_builtin(cmd, data, false));

	else
	{
		pid = fork(); // Fork the process
		if (pid == 0) // In child process
		{
			cmd_path = find_cmd_path(cmd->args);
			execve(cmd_path, cmd->args, data->env_vars); // Execute the command
			perror("minishell"); // If execve fails, print error
			exit(EXIT_FAILURE);  // Exit child process
		}
		else if (pid > 0) // In parent process
		{
			waitpid(pid, &cmd->exit_status, 0); // Wait for the child to finish
			if (WIFEXITED(cmd->exit_status))
			{
				data->last_exit_status = WEXITSTATUS(cmd->exit_status); // Update the last_exit_status
			}
			return (data->last_exit_status); // Return the exit status
		}
		else if (pid < 0) // If fork fails, print error
			perror("minishell: fork");
	}
	return (1); // Return 1 if something went wrong
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
    pid_t child_pids[256]; // Array to store PIDs of child processes (adjust size if needed)
    int num_children = 0;  // Counter for the number of child processes

    current = data->cmd_list;
    prev_fd = -1;

	if (current->name != NULL && strcmp(current->name, "exit") == 0 && current->next == NULL)
    {
        data->last_exit_status = execute_builtin(current, data, true); // Execute exit command directly
        free_cmd_list(data->cmd_list);
        return (data->last_exit_status); // Return exit status directly
    }
	// Execute built-in functions like export, unset (and cd?) directly in the parent process. 
	// Otherwise, env vars would only be modified in the child process, which then is terminated, and the env vars in the parent are unchanged
	if (current->name != NULL && ((strcmp(current->name, "export") == 0 || strcmp(current->name, "unset") == 0 )
			&& current->next == NULL))
    {
		data->last_exit_status = execute_builtin(current, data, false);
	}
	
    while (current != NULL) // Loop through each command in the list
    {

        if (current->next != NULL)
            pipe(pipe_fd); // Create a pipe if there's a next command

        pid = fork(); // Fork the process for the current command
        if (pid == 0) // In child process
        {
            if (prev_fd != -1) // If there's a previous command, set up input redirection
            {
                dup2(prev_fd, STDIN_FILENO); // Redirect input to previous pipe
                close(prev_fd);
            }
            if (current->next != NULL) // If there's a next command, set up output redirection
            {
                dup2(pipe_fd[1], STDOUT_FILENO); // Redirect output to next pipe
                close(pipe_fd[0]);
                close(pipe_fd[1]);
            }
            else if (current->output != NULL) // Handle output redirection for the last command
            {
                int fd_out = open(current->output, O_WRONLY | O_CREAT | (current->append_output ? O_APPEND : O_TRUNC), 0644);
                if (fd_out < 0) // Check for errors
                {
                    perror("minishell: output redirection");
                    free_cmd_list(data->cmd_list);  // Free memory in the child before exiting
                    free_tokens(data); // Free tokens in the child before exiting
                    exit(EXIT_FAILURE);
                }
                dup2(fd_out, STDOUT_FILENO); // Redirect standard output
                close(fd_out); // Close file descriptor
            }

            // Execute the command (built-in or external)
            if (is_builtin(current->name)) // If built-in, execute it in the child
            {
                // Check if the builtin is "exit" and there are other commands in the pipeline
                if (strcmp(current->name, "exit") == 0)
                {
                    // Free resources but don't terminate the shell
                    free_cmd_list(data->cmd_list);
                    free_tokens(data);
					data->last_exit_status = 1;
                    exit (data->last_exit_status); // Exit the child normally without terminating the shell
                }
				
				if (!((strcmp(current->name, "export") == 0) && !(strcmp(current->name, "unset") == 0 )))
                	data->last_exit_status = execute_builtin(current, data, false);

                if (data->exit_flag) // If it's exit, terminate the shell if there is no pipe
                {
                    free_cmd_list(data->cmd_list);
                    free_tokens(data);
                    exit(data->last_exit_status);
                }
                exit(data->last_exit_status); // Exit with the status of the built-in
            }
            else
            {
                data->last_exit_status = execute_single_cmd(current, data);
                free_cmd_list(data->cmd_list);  // Free memory in the child before exiting
                free_tokens(data);  // Free tokens in the child before exiting
                exit(data->last_exit_status);
            }
        }
        else if (pid > 0) // In parent process
        {
            child_pids[num_children++] = pid; // Store the PID of the child process
            if (prev_fd != -1)
                close(prev_fd); // Close previous pipe read end
            if (current->next != NULL)
            {
                close(pipe_fd[1]); // Close current pipe write end
                prev_fd = pipe_fd[0]; // Set up previous pipe read end for next iteration
            }
            else
                prev_fd = -1;
        }
        else
        {
            perror("minishell: fork"); // If fork fails, print error
            free_cmd_list(data->cmd_list);  // Free memory on error
            free_tokens(data);  // Free tokens on error
            return (1);
        }

        current = current->next; // Move to the next command in the list
    }

    // Wait for all child processes to finish
    for (int i = 0; i < num_children; i++)
    {
        waitpid(child_pids[i], &data->last_exit_status, 0); // Wait for each child
        if (WIFEXITED(data->last_exit_status))
            data->last_exit_status = WEXITSTATUS(data->last_exit_status); // Get the exit status of the last child process
    }

    free_cmd_list(data->cmd_list);  // Free memory in the parent after all children have finished
    free_tokens(data);  // Free tokens in the parent after all children have finished
    return (data->last_exit_status); // Return the exit status of the last command executed
}


/**
 * @brief Parses tokens into a linked list of command structures.
 *
 * This function processes an array of tokens generated by the lexer and
 * organizes them into a linked list of `t_command` structures. Each command
 * includes its name, arguments, input/output redirection, and other necessary
 * execution parameters.
 *
 * @param tokens Array of tokens to be parsed.
 * @param token_count The total number of tokens to parse.
 * @return t_command* The head of the linked list of commands.
 */
static t_command *parse_tokens(t_token *tokens, int token_count)
{
	t_command *cmd = malloc(sizeof(t_command));  // Allocate memory for the first command structure
	t_command *current_cmd = cmd;                // Set current command pointer to the first command
	int i = 0, arg_index = 0;                    // Initialize counters for token and argument indices

	// Initialize the first command's fields to default values
	current_cmd->name = NULL;                    // The name of the command is set later
	current_cmd->args = malloc(sizeof(char *) * (token_count + 1));  // Allocate memory for argument list
	current_cmd->input = NULL;                   // No input redirection by default
	current_cmd->output = NULL;                  // No output redirection by default
	current_cmd->append_output = 0;              // Default to not appending output
	current_cmd->exit_status = 0;                // Exit status set to 0
	current_cmd->next = NULL;                    // No next command yet

	// Iterate through all tokens
	while (i < token_count)
	{
		//printf("tokens[%d].type: %d\n", i, tokens[i].type);
		//printf("tokens[%d].value: %s\n", i, tokens[i].value);

		// Handle command words (non-operator tokens)
		if (tokens[i].type == TOKEN_WORD)
		{
			/*
			if (current_cmd->name == NULL)
				current_cmd->name = strdup(tokens[i].value);  // First word is the command name
			else
				current_cmd->args[arg_index++] = strdup(tokens[i].value);  // Subsequent words are arguments
			*/
			// corrected the above (as cmd args were constructed incorrectly, e.g. ls -la gave args[0] = "-la" )
			if (current_cmd->name == NULL)
				current_cmd->name = strdup(tokens[i].value);  // First word is the command name
			current_cmd->args[arg_index++] = strdup(tokens[i].value);  // First word and subsequent words are arguments
		}
		// Handle operators (such as pipes and redirection)
		else if (tokens[i].type == TOKEN_OPERATOR)
		{
			// Handle pipe operator: create a new command
			if (strcmp(tokens[i].value, "|") == 0)
			{
				current_cmd->next = malloc(sizeof(t_command));  // Allocate memory for the next command
				current_cmd = current_cmd->next;                // Move to the next command
				current_cmd->next = NULL;                       // Initialize the new command's next pointer
				current_cmd->args = malloc(sizeof(char *) * (token_count + 1));  // Allocate memory for arguments
				current_cmd->input = NULL;                      // Initialize input redirection to NULL
				current_cmd->output = NULL;                     // Initialize output redirection to NULL
				current_cmd->append_output = 0;                 // Default to no append mode
				current_cmd->exit_status = 0;                   // Initialize exit status to 0

				arg_index = 0;                                  // Reset argument index for the new command
			}
			// Handle output redirection (">")
			else if (strcmp(tokens[i].value, ">") == 0)
			{
				current_cmd->output = strdup(tokens[++i].value);  // Set the output file name from the next token
			}
			// Handle append output redirection (">>")
			else if (strcmp(tokens[i].value, ">>") == 0)
			{
				current_cmd->output = strdup(tokens[++i].value);  // Set the output file name
				current_cmd->append_output = 1;                   // Enable append mode
			}
			// Handle input redirection ("<")
			else if (strcmp(tokens[i].value, "<") == 0)
			{
				current_cmd->input = strdup(tokens[++i].value);   // Set the input file name from the next token
			}
		}
		i++;  // Move to the next token
	}

	// Null-terminate the argument list for the last command
	current_cmd->args[arg_index] = NULL;

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
// Function to duplicate the env_vars dynamically
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
	char		*temp_input;
	char		*full_input = NULL;
	int			in_quote;
	t_data		data;

	(void)argc;
	(void)argv;

	data.last_exit_status = 0;
	data.exit_flag = false;
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
		input = readline("minishell> ");

		// If the user inputs Ctrl-D (EOF), exit the shell
		if (input == NULL)
		{
			printf("Exiting minishell...\n");
			break;
		}

		if (*input)  // Only add non-empty input to history
			add_history(input);

		in_quote = check_for_unclosed_quotes(input);

		while (in_quote == 1)
		{
			temp_input = readline("> "); // Show a different prompt for continued input
			if (temp_input)
			{
				// Concatenate input with previous input
				full_input = ft_strjoin(input, "\n");
				full_input = ft_strjoin(full_input, temp_input);
				free(input);
				free(temp_input);
				input = full_input;
			}
			in_quote = check_for_unclosed_quotes(input);
		}

		// Count the number of tokens in the input string
		data.token_count = count_tokens(input);

		//printf("TOKEN COUNT is: %d\n", token_count);

		// Tokenize the input string and check for errors during lexing
		if (lexer(input, &data.tokens, data.token_count, data.last_exit_status) == -1)
		{
			printf("Error tokenizing input!\n");
			free(input); // Free input string on error
			free_array_of_strs(data.env_vars);
			return (1);  // Return 1 to indicate an error occurred
		}

		// Parse the tokens into a linked list of commands
		data.cmd_list = parse_tokens(data.tokens, data.token_count);

		// If parsing fails, print an error message and clean up
		if (!data.cmd_list)
		{
			printf("Error parsing commands!\n");
			free_tokens(&data);               // Free the token array
			free(input);                      // Free the input string
			free_array_of_strs(data.env_vars);
			return (1);                       // Return 1 to indicate an error occurred
		}

		// Execute the parsed list of commands and store the exit status of the last command
		data.last_exit_status = execute_cmd_list(&data);
		if (data.exit_flag == true)
		{
			free_tokens(&data); // Free the token array
			free(input);                      // Free the input string
			free_array_of_strs(data.env_vars);
			return (data.last_exit_status);        // Return 1 to indicate an error occurred
		}

		// Clean up the token array and free the input string
		free(input);
	}

	return (0); // Return 0 to indicate successful execution of the shell
}
