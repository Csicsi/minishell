#include "../includes/minishell.h"

static int	is_valid_env_var_name(const char *name)
{
	if (!ft_isalpha(*name) && *name != '_')
		return (0);
	while (*name)
	{
		if (!ft_isalnum(*name) && *name != '_')
			return (0);
		name++;
	}
	return (1);
}

void	sort_env_vars(char **env_vars, int count)
{
	int		i;
	int		j;
	char	*temp;

	i = 0;
	while (i < count - 1)
	{
		j = i + 1;
		while (j < count)
		{
			if (ft_strcmp(env_vars[i], env_vars[j]) > 0)
			{
				temp = env_vars[i];
				env_vars[i] = env_vars[j];
				env_vars[j] = temp;
			}
			j++;
		}
		i++;
	}
}

void	print_sorted_env_vars(char **env_vars)
{
	int		i;
	char	*equal_sign;

	i = 0;
	while (env_vars[i] != NULL)
	{
		equal_sign = ft_strchr(env_vars[i], '=');
		if (equal_sign)
		{
			printf("declare -x ");
			printf("%.*s", (int)(equal_sign - env_vars[i]), env_vars[i]);
			printf("=\"%s\"\n", equal_sign + 1);
		}
		else
		{
			printf("declare -x %s\n", env_vars[i]);
		}
		i++;
	}
}

static int	handle_export_wo_args(t_command *cmd, t_data *data)
{
	int		env_count;
	char	**sorted_env_vars;
	int		i;

	(void)cmd;
	env_count = 0;
	while (data->env_vars[env_count] != NULL)
		env_count++;
	sorted_env_vars = malloc((env_count + 1) * sizeof(char *));
	if (!sorted_env_vars)
		return (1);
	i = 0;
	while (i < env_count)
	{
		sorted_env_vars[i] = data->env_vars[i];
		i++;
	}
	sorted_env_vars[env_count] = NULL;
	sort_env_vars(sorted_env_vars, env_count);
	print_sorted_env_vars(sorted_env_vars);
	free(sorted_env_vars);
	return (0);
}

int	builtin_export(t_command *cmd, t_data *data)
{
	int		i;
	int		arg;
	char	*equal_sign;
	char	*varname;
	char	*varname_value;
	int		encountered_invalid_varname;
	int		env_count;

	encountered_invalid_varname = 0;
	if (cmd->args[1] == NULL)
		return (handle_export_wo_args(cmd, data));
	arg = 1;
	while (cmd->args[arg] != NULL)
	{
		varname_value = cmd->args[arg];
		equal_sign = ft_strchr(varname_value, '=');
		if (equal_sign)
			varname = ft_substr(varname_value, 0, equal_sign - varname_value);
		else
			varname = strdup(varname_value);
		if (!is_valid_env_var_name(varname) || *varname == '\0')
		{
			fprintf(stderr, ": export: `%s': not a valid identifier\n",
				varname_value);
			free(varname);
			encountered_invalid_varname = 1;
			arg++;
			continue ;
		}
		i = 0;
		while (data->env_vars[i] != NULL)
		{
			if (ft_strncmp(data->env_vars[i], varname, ft_strlen(varname)) == 0
				&& (data->env_vars[i][ft_strlen(varname)] == '='
				|| data->env_vars[i][ft_strlen(varname)] == '\0'))
				break ;
			i++;
		}
		if (data->env_vars[i] != NULL)
		{
			if (ft_strchr(varname_value, '='))
			{
				free(data->env_vars[i]);
				data->env_vars[i] = strdup(varname_value);
				if (!data->env_vars[i])
					return (1);
			}
		}
		else
		{
			env_count = i;
			data->env_vars = realloc(data->env_vars,
					(env_count + 2) * sizeof(char *));
			if (!data->env_vars)
				return (1);
			data->env_vars[env_count] = strdup(varname_value);
			if (!data->env_vars[env_count])
				return (1);
			data->env_vars[env_count + 1] = NULL;
		}
		free(varname);
		arg++;
	}
	if (encountered_invalid_varname == 1)
		return (1);
	return (0);
}
