#include "../includes/minishell.h"

int	update_last_command_env_var(t_data *data, char *cmd_path)
{
	int		i;
	char	*new_var;
	char	**new_env_vars;

	i = 0;
	new_var = ft_strjoin("_=", cmd_path);
	while (data->env_vars[i])
	{
		if (ft_strncmp(data->env_vars[i], "_=", 2) == 0)
		{
			free(data->env_vars[i]);
			data->env_vars[i] = new_var;
			return (0);
		}
		i++;
	}
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
