/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd_utils.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krabitsc <krabitsc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/19 17:24:52 by krabitsc          #+#    #+#             */
/*   Updated: 2024/10/19 20:13:05 by krabitsc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

int	ft_setenv(const char *varname, const char *value, t_data *data)
{
	int		i;
	char	*varname_value;
	int		len_varname;
	int		len_value;
	int		env_count;

	len_varname = ft_strlen(varname);
	len_value = ft_strlen(value);
	varname_value = malloc(len_varname + len_value + 2);
	if (!varname_value)
		return (1);
	ft_strlcpy(varname_value, varname, len_varname + 1);
	varname_value[len_varname] = '=';
	ft_strlcpy(varname_value + len_varname + 1, value, len_value + 1);
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
			data->env_vars[i] = ft_strdup(varname_value);
			if (!data->env_vars[i])
				return (1);
			free(varname_value);
		}
	}
	else
	{
		env_count = i;
		data->env_vars = ft_realloc(data->env_vars, (env_count + 1) * sizeof(char *), (env_count + 2) * sizeof(char *));
		if (!data->env_vars)
			return (1);
		data->env_vars[env_count] = ft_strdup(varname_value);
		if (!data->env_vars[env_count])
			return (1);
		data->env_vars[env_count + 1] = NULL;
		free(varname_value);
	}
	return (0);
}

static void	concatenate_path(char *result, const char *dir, const char *arg)
{
	while (*dir)
		*result++ = *dir++;
	if (*(result - 1) != '/')
		*result++ = '/';
	while (*arg)
		*result++ = *arg++;
	*result = '\0';
}

static char	*resolve_cdpath(const char *cdpath, const char *arg)
{
	char	temp_path[PATH_MAX];
	char	*token;
	int		token_len;

	token = (char *)cdpath;
	while (*token)
	{
		token_len = 0;
		while (token[token_len] && token[token_len] != ':')
			token_len++;
		if (token_len < PATH_MAX)
		{
			ft_strlcpy(temp_path, token, token_len + 1);
			concatenate_path(temp_path, temp_path, arg);
			if (access(temp_path, F_OK) == 0)
				return (ft_strdup(temp_path));
		}
		token += token_len;
		if (*token == ':')
			token++;
	}
	return (NULL);
}

char	*normalize_path(const char *path)
{
	char	normalized_path[PATH_MAX];

	if (realpath(path, normalized_path) == NULL)
	/*
	printf("input: path: %s\n", path);
	char *test = realpath(path, normalized_path);
	printf("comparison, test: %s\n", test);
	if (ft_realpath(path, normalized_path) == NULL)
	*/
	{
		fprintf(stderr, ": cd: %s: No such file or directory\n", path);
		return (NULL);
	}
	return (ft_strdup(normalized_path));
}

char	*resolve_cdpath_if_needed(const char *path, t_data *data)
{
	char	*cdpath;

	cdpath = ft_getenv(ft_strdup("CDPATH"), data->env_vars);
	if (cdpath != NULL)
		return (resolve_cdpath(cdpath, path));
	return (NULL);
}

int	update_directory_env(const char *cwd, t_data *data)
{
	(void)data;
	if (setenv("PWD", cwd, 1) != 0)
	{
		perror(": cd: setenv PWD");
		return (1);
	}
	return (0);
}
