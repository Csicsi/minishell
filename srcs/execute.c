/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: csicsi <csicsi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/14 14:31:51 by dcsicsak          #+#    #+#             */
/*   Updated: 2024/08/14 18:00:01 by csicsi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

int	builtin_cd(t_command *cmd)
{
	(void)cmd;
	printf("builtin_cd: Changing directory...\n");
	return (0);
}

int	builtin_echo(t_command *cmd)
{
	(void)cmd;
	printf("builtin_echo: \n");
	return (0);
}

int	builtin_exit(t_command *cmd)
{
	(void)cmd;
	printf("builtin_exit: Exiting shell...\n");
	return (0);
}

int	builtin_env(t_command *cmd)
{
	(void)cmd;
	printf("builtin_env: Displaying environment variables...\n");
	return (0);
}

int	builtin_export(t_command *cmd)
{
	(void)cmd;
	printf("builtin_export: Exporting environment variable...\n");
	return (0);
}

int	builtin_unset(t_command *cmd)
{
	(void)cmd;
	printf("builtin_unset: Unsetting environment variable...\n");
	return (0);
}

int	builtin_pwd(t_command *cmd)
{
	(void)cmd;
	printf("builtin_pwd: Printing working directory...\n");
	return (0);
}

int	is_builtin(char *command_name) // return 1 if command is a builtin, 0 if not
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

// Execute the appropriate builtin command based on the command name
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
	return (1); // Return 1 if the command is not a recognized builtin, for safety
}

void	execute_single_cmd(t_command *cmd)
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
			return ;
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
			return ;
		}
		dup2(fd_out, STDOUT_FILENO); // Redirect standard output
		close(fd_out); // Close file descriptor
	}

	// Check if the command is a builtin
	if (is_builtin(cmd->name))
		cmd->exit_status = execute_builtin(cmd); // Execute builtin command
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
			waitpid(pid, &cmd->exit_status, 0);
		else if (pid < 0) // If fork fails, print error
			perror("minishell: fork");
	}
}

// Function to execute a list of commands, handling piping and redirection
void	execute_cmd_list(t_command *cmd)
{
	t_command	*current;
	int			pipe_fd[2];
	int			prev_fd;
	pid_t		pid;

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
			execute_single_cmd(current); // Execute the current command
			exit(current->exit_status); // Exit with the command's status
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
				waitpid(pid, &current->exit_status, 0); // Wait for child process if not background
		}
		else
			perror("minishell: fork"); // If fork fails, print error
		current = current->next; // Move to the next command in the list
	}
}

// Main function to set up and execute commands
int	main(int argc, char **argv, char **envp)
{
	t_command	cmd1;
	t_command	cmd2;

	(void)argc;
	(void)argv;

	///bin/ls -la | /usr/bin/grep e > output.txt
	// Set up the first command: "ls -la"
	cmd1.name = "/bin/ls";
	cmd1.args = (char*[]){"/bin/ls", "-la", NULL};
	cmd1.input = NULL;
	cmd1.output = NULL;
	cmd1.append_output = 0;
	cmd1.background = 0;
	cmd1.exit_status = 0;
	cmd1.next = &cmd2; // Link to the next command
	cmd1.env_vars = envp;

	// Set up the second command: "grep e", output to "output.txt"
	cmd2.name = "/usr/bin/grep";
	cmd2.args = (char*[]){"/usr/bin/grep", "e", NULL};
	cmd2.input = NULL;
	cmd2.output = "output.txt";
	cmd2.append_output = 0;
	cmd2.background = 0;
	cmd2.exit_status = 0;
	cmd2.next = NULL; // No next command, end of list
	cmd2.env_vars = envp;

	// Execute the list of commands
	execute_cmd_list(&cmd1);

	return (0); // Return success status
}
