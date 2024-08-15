/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcsicsak <dcsicsak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/14 14:31:51 by dcsicsak          #+#    #+#             */
/*   Updated: 2024/08/15 16:12:28 by dcsicsak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

/**
 * @brief Handles the `echo` builtin command.
 *
 * @param cmd The command structure containing arguments.
 * @return int Always returns 0 for success.
 */
int	builtin_echo(t_command *cmd)
{
	(void)cmd; // Ignore unused parameter
	printf("builtin_echo: \n"); // Placeholder message
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
 * @brief Handles the `pwd` builtin command.
 *
 * @param cmd The command structure containing arguments.
 * @return int Always returns 0 for success.
 */
int	builtin_pwd(t_command *cmd)
{
	(void)cmd; // Ignore unused parameter
	printf("builtin_pwd: Printing working directory...\n"); // Placeholder message
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
 * @brief Parses the command-line arguments into a command structure.
 *
 * @param cmd The command structure to fill.
 * @param argv The array of command-line arguments.
 * @param index Pointer to the current argument index.
 * @param envp The environment variables.
 */
void	parse_command(t_command *cmd, char **argv, int *index, char **envp)
{
	cmd->name = argv[*index];
	cmd->args = &argv[*index];
	cmd->input = NULL;
	cmd->output = NULL;
	cmd->append_output = 0;
	cmd->background = 0;
	cmd->exit_status = 0;
	cmd->env_vars = envp;
	while (argv[*index] != NULL)
		(*index)++;
	cmd->next = NULL;
}

/**
 * @brief The main entry point for the shell program.
 *
 * @param argc The number of command-line arguments.
 * @param argv The array of command-line arguments.
 * @param envp The environment variables.
 * @return int The exit status of the shell.
 */
int	main(int argc, char **argv, char **envp)
{
	t_command	cmd1;
	t_command	cmd2;
	int			command_count;
	int			index;
	int			last_exit_status;

	if (argc < 3)
	{
		fprintf(stderr, "Usage: %s <1|2> <command1> [args]\n", argv[0]);
		return (1);
	}

	// Determine if there is one command or two
	command_count = atoi(argv[1]);
	if (command_count != 1 && command_count != 2)
	{
		fprintf(stderr, "Error: First argument must be 1 or 2.\n");
		return (1);
	}

	index = 2; // Start after the first two arguments
	parse_command(&cmd1, argv, &index, envp);

	// If two commands are specified, set up the hardcoded second command
	if (command_count == 2)
	{
		cmd2.name = "/usr/bin/grep";
		cmd2.args = (char*[]){"/usr/bin/grep", "e", NULL};
		cmd2.input = NULL;
		cmd2.output = "output.txt";
		cmd2.append_output = 0;
		cmd2.background = 0;
		cmd2.exit_status = 0;
		cmd2.next = NULL; // No next command, end of list
		cmd2.env_vars = envp;

		cmd1.next = &cmd2; // Link the first command to the hardcoded second command
	}

	// Execute the list of commands
	last_exit_status = execute_cmd_list(&cmd1);

	// Return the exit status of the last command
	if (WIFEXITED(last_exit_status))
		return (WEXITSTATUS(last_exit_status));
	else
		return (1); // Return 1 if the process terminated abnormally
}
