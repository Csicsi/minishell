#include "../includes/minishell.h"

int	builtin_unset(t_command *cmd, t_data *data)
{
	int		i;
	int		j;
	int		arg;

	arg = 1;
	while (cmd->args[arg] != NULL)
	{
		i = 0;
		while (data->env_vars[i] != NULL)
		{
			if (ft_strncmp(data->env_vars[i], cmd->args[arg],
					ft_strlen(cmd->args[arg])) == 0
				&& (data->env_vars[i][ft_strlen(cmd->args[arg])] == '='
				|| data->env_vars[i][ft_strlen(cmd->args[arg])] == '\0'))
			{
				free(data->env_vars[i]);
				j = i;
				while (data->env_vars[j] != NULL)
				{
					data->env_vars[j] = data->env_vars[j + 1];
					j++;
				}
				break ;
			}
			i++;
		}
		arg++;
	}
	return (0);
}
