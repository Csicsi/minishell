#include "../includes/minishell.h"

static int	find_and_update_var(char **env_vars,
	const char *varname, const char *varname_value)
{
	int		i;
	int		varname_len;

	varname_len = ft_strlen(varname);
	i = 0;
	while (env_vars && env_vars[i] != NULL)
	{
		if (ft_strncmp(env_vars[i], varname, varname_len) == 0
			&& (env_vars[i][varname_len] == '='
			|| env_vars[i][varname_len] == '\0'))
		{
			if (ft_strchr(varname_value, '='))
			{
				free(env_vars[i]);
				env_vars[i] = ft_strdup(varname_value);
				if (!env_vars[i])
					return (1);
			}
			return (0);
		}
		i++;
	}
	return (2);
}

static int	add_new_var(char ***env_vars, const char *varname_value)
{
	int		env_count;
	char	**new_env_vars;

	if (*env_vars == NULL)
	{
		*env_vars = ft_calloc(1, sizeof(char *));
		if (!*env_vars)
			return (1);
	}
	env_count = 0;
	while ((*env_vars)[env_count] != NULL)
		env_count++;
	new_env_vars = ft_realloc(*env_vars,
			(env_count + 1) * sizeof(char *),
			(env_count + 2) * sizeof(char *));
	if (!new_env_vars)
		return (1);
	new_env_vars[env_count] = ft_strdup(varname_value);
	if (!new_env_vars[env_count])
		return (free(new_env_vars), 1);
	new_env_vars[env_count + 1] = NULL;
	*env_vars = new_env_vars;
	return (0);
}

static int	handle_invalid_identifier(const char *varname_value)
{
	ft_fprintf(2, ": export: `%s': not a valid identifier\n",
		varname_value);
	return (1);
}

static int	process_export_argument(const char *varname_value, t_data *data)
{
	char	*varname;
	char	*equal_sign;
	int		ret;

	equal_sign = ft_strchr(varname_value, '=');
	if (equal_sign)
		varname = ft_substr(varname_value, 0, equal_sign - varname_value);
	else
		varname = ft_strdup(varname_value);
	if (!is_valid_env_var_name(varname) || *varname == '\0')
	{
		free(varname);
		return (handle_invalid_identifier(varname_value));
	}
	ret = find_and_update_var(data->env_vars, varname, varname_value);
	if (ret == 2)
		ret = add_new_var(&data->env_vars, varname_value);
	free(varname);
	return (ret);
}

int	builtin_export(t_cmd *cmd, t_data *data)
{
	int		arg;
	int		encountered_invalid_varname;

	encountered_invalid_varname = 0;
	if (cmd->args[1] == NULL)
		return (handle_export_wo_args(cmd, data));
	arg = 1;
	while (cmd->args && cmd->args[arg] != NULL)
	{
		if (process_export_argument(cmd->args[arg], data))
			encountered_invalid_varname = 1;
		arg++;
	}
	if (encountered_invalid_varname == 1)
		return (1);
	return (0);
}
