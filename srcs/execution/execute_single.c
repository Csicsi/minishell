#include "../includes/minishell.h"

/*
static int	handle_command_not_found(t_cmd *cmd, t_data *data, char **cmd_path)
{
	data->last_exit_status = 127;
	if (cmd->args[0] == NULL && cmd->output)
		return (data->last_exit_status);
	if (access(cmd->args[0], F_OK) == 0 && access(cmd->args[0], X_OK) != 0)
	{
		data->last_exit_status = 126;
		ft_fprintf(2, ": %s: Permission denied\n", cmd->args[0]);
		return (data->last_exit_status);
	}
	if (ft_strchr(cmd->args[0], '/')
		|| (*cmd_path && **cmd_path == '\0'))
		ft_fprintf(2, ": %s: No such file or directory\n", cmd->args[0]);
	else
		ft_fprintf(2, ": %s: command not found\n", cmd->args[0]);
	return (data->last_exit_status);
}

static int	setup_command_execution(t_cmd *cmd, t_data *data, char **cmd_path)
{
	int err_flag ;

	err_flag  = 0;
	if (handle_input_redirection(cmd, data) < 0)
		return (data->last_exit_status);
	if (handle_output_redirection(cmd, data) < 0)
		return (data->last_exit_status);
	if (is_builtin(cmd->args[0]))
	{
		data->last_exit_status = execute_builtin(cmd, data, false);
		return (data->last_exit_status);
	}
	*cmd_path = find_cmd_path(cmd->args, data, &err_flag);
	//if (!*cmd_path || **cmd_path == '\0')
	//	return (handle_command_not_found(cmd, data, cmd_path));
	if (err_flag == -1)
		return (handle_command_not_found(cmd, data, cmd_path));
	if (update_last_command_env_var(data, *cmd_path) == -1)
	{
		free(*cmd_path);
		data->last_exit_status = 1;
		return (1);
	}
	return (0);
}
*/

static int	handle_command_not_found(t_cmd *cmd, t_data *data, char **cmd_path)
{
	data->last_exit_status = 127;
	if (cmd->args[0] == NULL)
		return (data->last_exit_status);
	if (ft_strchr(cmd->args[0], '/')
		|| (*cmd_path && **cmd_path == '\0'))
		ft_fprintf(2, ": %s: No such file or directory\n", cmd->args[0]);
	else
		ft_fprintf(2, ": %s: command not found\n", cmd->args[0]);
	return (data->last_exit_status);
}

static int	setup_command_execution(t_cmd *cmd,
	t_data *data, t_exec_context *ctx, char **cmd_path)
{
	if (cmd->skip_execution || cmd->empty_redir)
		return (1);
	if (!cmd->args[0] && cmd->is_heredoc)
	{
		data->last_exit_status = 0;
		return (1);
	}
	if (is_builtin(cmd->args[0]))
	{
		data->last_exit_status = execute_builtin(cmd, data, ctx, false);
		return (data->last_exit_status);
	}
	*cmd_path = find_cmd_path(cmd->args, data);
	if (!*cmd_path || **cmd_path == '\0')
		return (handle_command_not_found(cmd, data, cmd_path));
	if (update_last_command_env_var(data, *cmd_path) == -1)
	{
		free(*cmd_path);
		data->last_exit_status = 1;
		return (1);
	}
	return (0);
}

int	execute_single_cmd(t_cmd *cmd, t_data *data, t_exec_context *ctx)
{
	char	*cmd_path;

	cmd_path = NULL;
	if (setup_command_execution(cmd, data, ctx, &cmd_path) != 0)
		return (data->last_exit_status);
	if (!cmd->skip_execution && !cmd->empty_redir)
	{
		if (execve(cmd_path, cmd->args, data->env_vars) == -1)
		{
			if (ft_strchr(cmd->args[0], '/'))
				ft_fprintf(2, ": %s: Is a directory\n", cmd->args[0]);
			else
				ft_fprintf(2, ": %s: command not found\n", cmd->args[0]);
			if (ft_strchr(cmd->args[0], '/'))
				data->last_exit_status = 126;
			else
				data->last_exit_status = 127;
			free(cmd_path);
			return (data->last_exit_status);
		}
	}
	free(cmd_path);
	return (0);
}
