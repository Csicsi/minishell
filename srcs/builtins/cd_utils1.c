/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd_utils1.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krabitsc <krabitsc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/20 10:55:01 by krabitsc          #+#    #+#             */
/*   Updated: 2024/10/20 10:55:11 by krabitsc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

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

char	*resolve_cdpath_if_needed(const char *path, t_data *data)
{
	char	*cdpath;

	cdpath = ft_getenv(ft_strdup("CDPATH"), data->env_vars);
	if (cdpath != NULL)
		return (resolve_cdpath(cdpath, path));
	return (NULL);
}
