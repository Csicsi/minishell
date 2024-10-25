#include "../includes/minishell.h"

static int	update_existing_env_var(char **env_vars, char *new_var)
{
	int	i;

	i = 0;
	while (env_vars[i])
	{
		if (ft_strncmp(env_vars[i], "_=", 2) == 0)
		{
			free(env_vars[i]);
			env_vars[i] = new_var;
			return (1);
		}
		i++;
	}
	return (0);
}

static int	add_new_env_var(t_data *data, char *new_var)
{
	int		i;
	char	**new_env_vars;

	i = 0;
	while (data->env_vars[i])
		i++;
	new_env_vars = malloc(sizeof(char *) * (i + 2));
	if (!new_env_vars)
		return (-1);
	i = 0;
	while (data->env_vars[i])
	{
		new_env_vars[i] = data->env_vars[i];
		i++;
	}
	new_env_vars[i] = new_var;
	new_env_vars[i + 1] = NULL;
	free(data->env_vars);
	data->env_vars = new_env_vars;
	return (0);
}

int	update_last_command_env_var(t_data *data, char *cmd_path)
{
	char	*new_var;

	new_var = ft_strjoin("_=", cmd_path);
	if (update_existing_env_var(data->env_vars, new_var))
		return (0);
	return (add_new_env_var(data, new_var));
}

char	*get_directory_from_path(const char *path)
{
	int		i;
	char	*dir;

	i = ft_strlen(path) - 1;
	while (i >= 0 && path[i] != '/')
		i--;
	if (i < 0)
		return (NULL);
	dir = ft_strndup(path, i);
	if (!dir)
		return (NULL);
	return (dir);
}

t_token	*find_token_by_value(t_token *tokens, const char *value)
{
	t_token	*current;

	current = tokens;
	while (current != NULL)
	{
		if (ft_strcmp(current->value, value) == 0)
			return (current);
		current = current->next;
	}
	return (NULL);
}
