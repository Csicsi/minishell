/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcsicsak <dcsicsak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/14 14:31:51 by dcsicsak          #+#    #+#             */
/*   Updated: 2024/08/14 15:25:16 by dcsicsak         ###   ########.fr       */
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
	return (1);
}

void	execute_single_cmd(t_command *cmd)
{
	int		fd_in;
	int		fd_out;
	pid_t	pid;

	if (cmd->input != NULL)
	{
		fd_in = open(cmd->input, O_RDONLY);
		if (fd_in < 0)
		{
			perror("minishell: input redirection");
			return ;
		}
		dup2(fd_in, STDIN_FILENO);
		close(fd_in);
	}
	if (cmd->output != NULL)
	{
		fd_out = open(cmd->output, O_WRONLY | O_CREAT | (cmd->append_output ? O_APPEND : O_TRUNC), 0644);
		if (fd_out < 0)
		{
			perror("minishell: output redirection");
			return ;
		}
		dup2(fd_out, STDOUT_FILENO);
		close(fd_out);
	}
	if (is_builtin(cmd->name))
		cmd->exit_status = execute_builtin(cmd);
	else
	{
		pid = fork();
		if (pid == 0)
		{
			execve(cmd->name, cmd->args, cmd->env_vars);
			perror("minishell");
			exit(EXIT_FAILURE);
		}
		else if (pid > 0 && !cmd->background)
			waitpid(pid, &cmd->exit_status, 0);
		else if (pid < 0)
			perror("minishell: fork");
	}
}

void	execute_cmd_list(t_command *cmd)
{
	t_command	*current;
	int			pipe_fd[2];
	int			prev_fd;
	pid_t		pid;

	current = cmd;
	prev_fd = -1;
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
				int fd_out = open(current->output, O_WRONLY | O_CREAT | (current->append_output ? O_APPEND : O_TRUNC), 0644);
				if (fd_out < 0)
				{
					perror("minishell: output redirection");
					exit(EXIT_FAILURE);
				}
				dup2(fd_out, STDOUT_FILENO);
				close(fd_out);
			}
			execute_single_cmd(current);
			exit(current->exit_status);
		}
		else if (pid > 0)
		{
			if (prev_fd != -1)
				close(prev_fd);
			if (current->next != NULL)
			{
				close(pipe_fd[1]);
				prev_fd = pipe_fd[0];
			}
			else
				prev_fd = -1;
			if (!current->background)
				waitpid(pid, &current->exit_status, 0);
		}
		else
			perror("minishell: fork");
		current = current->next;
	}
}

int	main(int argc, char **argv, char **envp)
{
	t_command	cmd1;
	t_command	cmd2;

	(void)argc;
	(void)argv;
	cmd1.name = "/bin/ls";
	cmd1.args = (char*[]){"/bin/ls", "-la", NULL};
	cmd1.input = NULL;
	cmd1.output = NULL;
	cmd1.append_output = 0;
	cmd1.background = 0;
	cmd1.exit_status = 0;
	cmd1.next = &cmd2;
	cmd1.env_vars = envp;
	cmd2.name = "/usr/bin/grep";
	cmd2.args = (char*[]){"/usr/bin/grep", "e", NULL};
	cmd2.input = NULL;
	cmd2.output = "output.txt";
	cmd2.append_output = 0;
	cmd2.background = 0;
	cmd2.exit_status = 0;
	cmd2.next = NULL;
	cmd2.env_vars = envp;
	execute_cmd_list(&cmd1);
	return (0);
}
