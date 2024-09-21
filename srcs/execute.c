/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcsicsak <dcsicsak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/14 14:31:51 by dcsicsak          #+#    #+#             */
/*   Updated: 2024/09/21 17:27:47 by dcsicsak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/**
 * @brief Handles the `cd` builtin command.
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
 * @brief Handles the `exit` builtin command.
 *
 * @param cmd The command structure containing arguments.
 * @return int Always returns 0 for success.
 */
int	builtin_exit(t_command *cmd)
{
	(void)cmd; // Ignore unused parameter
	printf("builtin_exit: Exiting shell...\n"); // Placeholder message
	return (0);
}

/**
 * @brief Handles the `env` builtin command.
 *
 * @param cmd The command structure containing arguments.
 * @return int Always returns 0 for success.
 */
int	builtin_env(t_command *cmd)
{
	(void)cmd; // Ignore unused parameter
	printf("builtin_env: Displaying environment variables...\n"); // Placeholder message
	return (0);
}

/**
 * @brief Handles the `export` builtin command.
 *
 * @param cmd The command structure containing arguments.
 * @return int Always returns 0 for success.
 */
int	builtin_export(t_command *cmd)
{
	(void)cmd; // Ignore unused parameter
	printf("builtin_export: Exporting environment variable...\n"); // Placeholder message
	return (0);
}

/**
 * @brief Handles the `unset` builtin command.
 *
 * @param cmd The command structure containing arguments.
 * @return int Always returns 0 for success.
 */
int	builtin_unset(t_command *cmd)
{
	(void)cmd; // Ignore unused parameter
	printf("builtin_unset: Unsetting environment variable...\n"); // Placeholder message
	return (0);
}

/**
 * @brief Checks if a command is a recognized builtin.
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
 * @param cmd The command structure containing arguments.
 * @return int The exit status of the executed builtin command.
 */
int	execute_builtin(t_command *cmd)
{
	if (ft_strncmp(cmd->name, "cd", 3) == 0)
		return (builtin_cd(cmd));
	else if (ft_strncmp(cmd->name, "echo", 5) == 0)
		return (builtin_echo(cmd));
	else if (ft_strncmp(cmd->name, "exit", 5) == 0)
		return (builtin_exit(cmd));
	else if (ft_strncmp(cmd->name, "env", 4) == 0)
		return (builtin_env(cmd));
	else if (ft_strncmp(cmd->name, "export", 7) == 0)
		return (builtin_export(cmd));
	else if (ft_strncmp(cmd->name, "unset", 6) == 0)
		return (builtin_unset(cmd));
	else if (ft_strncmp(cmd->name, "pwd", 4) == 0)
		return (builtin_pwd(cmd));
	return (1); // Return 1 if the command is not recognized as a builtin
}

/**
 * @brief Executes a single command, handling input/output redirection and builtins.
 *
 * @param cmd The command structure containing arguments.
 * @return int The exit status of the executed command.
 */
int	execute_single_cmd(t_command *cmd)
{
	int		fd_in;
	int		fd_out;
	pid_t	pid;

	printf("Executing command: %s\n", cmd->name);
	for (int i = 0; cmd->args && cmd->args[i] != NULL; i++)
	{
		printf("Argument %d: %s\n", i, cmd->args[i]);
	}

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
		fd_out = open(cmd->output, O_WRONLY | O_CREAT | (cmd->append_output ? O_APPEND : O_TRUNC), 0644);
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
		return (execute_builtin(cmd));
	else
	{
		pid = fork(); // Fork the process
		if (pid == 0) // In child process
		{
			execve(cmd->name, cmd->args, cmd->env_vars); // Execute external command
			perror("minishell"); // If execve fails, print error
			exit(EXIT_FAILURE); // Exit child process
		}
		else if (pid > 0 && !cmd->background) // In parent process, wait for child if not background
		{
			waitpid(pid, &cmd->exit_status, 0);
			if (WIFEXITED(cmd->exit_status))
				return (WEXITSTATUS(cmd->exit_status)); // Return the exit status of the command
		}
		else if (pid < 0) // If fork fails, print error
			perror("minishell: fork");
	}
	return (1); // Return 1 if something went wrong
}

/**
 * @brief Executes a list of commands with optional piping and redirection.
 *
 * @param cmd The first command in the linked list of commands.
 * @return int The exit status of the last command executed.
 */
int	execute_cmd_list(t_command *cmd)
{
	t_command	*current;
	int			pipe_fd[2];
	int			prev_fd;
	pid_t		pid;
	int			exit_status;

	current = cmd;
	prev_fd = -1;
	while (current != NULL) // Loop through each command in the list
	{
		if (current->next != NULL)
			pipe(pipe_fd); // Create a pipe if there's a next command

		pid = fork(); // Fork the process for the current command
		if (pid == 0) // In child process
		{
			if (prev_fd != -1) // If there's a previous command, set up input redirection
			{
				dup2(prev_fd, STDIN_FILENO);
				close(prev_fd);
			}
			if (current->next != NULL) // If there's a next command, set up output redirection
			{
				dup2(pipe_fd[1], STDOUT_FILENO);
				close(pipe_fd[0]);
				close(pipe_fd[1]);
			}
			else if (current->output != NULL) // Handle output redirection for the last command
			{
				int fd_out = open(current->output, O_WRONLY | O_CREAT | (current->append_output ? O_APPEND : O_TRUNC), 0644);
				if (fd_out < 0) // Check for errors
				{
					perror("minishell: output redirection");
					exit(EXIT_FAILURE);
				}
				dup2(fd_out, STDOUT_FILENO); // Redirect standard output
				close(fd_out); // Close file descriptor
			}
			exit(execute_single_cmd(current)); // Execute the current command and exit with its status
		}
		else if (pid > 0) // In parent process
		{
			if (prev_fd != -1)
				close(prev_fd); // Close previous pipe read end
			if (current->next != NULL)
			{
				close(pipe_fd[1]); // Close current pipe write end
				prev_fd = pipe_fd[0]; // Set up previous pipe read end for next iteration
			}
			else
				prev_fd = -1;
			if (!current->background)
				waitpid(pid, &exit_status, 0); // Wait for child process if not background
		}
		else
			perror("minishell: fork"); // If fork fails, print error

		current = current->next; // Move to the next command in the list
	}
	return (exit_status); // Return the exit status of the last command executed
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
	current_cmd->env_vars = NULL;                // No environment variables yet

	// Iterate through all tokens
	while (i < token_count)
	{
		// Handle command words (non-operator tokens)
		if (tokens[i].type == TOKEN_WORD)
		{
			if (current_cmd->name == NULL)
				current_cmd->name = strdup(tokens[i].value);  // First word is the command name
			else
				current_cmd->args[arg_index++] = strdup(tokens[i].value);  // Subsequent words are arguments
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
				current_cmd->env_vars = NULL;                   // No environment variables for this command yet
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
 * @brief Entry point for the minishell program.
 *
 * The main function is responsible for running the main input loop of the shell,
 * reading user input, tokenizing it, parsing it into commands, and executing those
 * commands.
 *
 * @return int Exit status of the program (0 for success, 1 for errors).
 */
int	main(void)
{
	char		*input;         // Input string entered by the user
	t_token		*tokens = NULL; // Array of tokens generated by the lexer
	t_command	*cmd_list;      // Linked list of parsed commands
	int			token_count;    // Number of tokens in the input
	int			last_exit_status; // Exit status of the last command executed

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

		// If the user types "exit", break the loop and exit the shell
		if (strcmp(input, "exit") == 0)
		{
			printf("Exiting minishell...\n");
			free(input); // Free the input string before exiting
			break;
		}

		// Count the number of tokens in the input string
		token_count = count_tokens(input);

		// Tokenize the input string and check for errors during lexing
		if (lexer(input, &tokens, token_count) == -1)
		{
			printf("Error tokenizing input!\n");
			free(input); // Free input string on error
			return (1);  // Return 1 to indicate an error occurred
		}

		// Parse the tokens into a linked list of commands
		cmd_list = parse_tokens(tokens, token_count);

		// If parsing fails, print an error message and clean up
		if (!cmd_list)
		{
			printf("Error parsing commands!\n");
			free_tokens(tokens, token_count); // Free the token array
			free(input);                      // Free the input string
			return (1);                       // Return 1 to indicate an error occurred
		}

		// Execute the parsed list of commands and store the exit status of the last command
		last_exit_status = execute_cmd_list(cmd_list);

		// Clean up the token array and free the input string
		free_tokens(tokens, token_count);
		free(input);
	}

	return (0); // Return 0 to indicate successful execution of the shell
}
