/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd_utils3.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krabitsc <krabitsc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/13 10:55:40 by krabitsc          #+#    #+#             */
/*   Updated: 2024/11/13 15:23:50 by krabitsc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

char	*ft_realpath(const char *path, char *resolved_path)
{
	char		tmp[PATH_MAX];
	char		**path_parts;
	int			i;
	struct stat	statbuf;
	char		*last_slash;

	if (path == NULL)
		return (NULL);
	if (path[0] == '/')
	{
		i = 0;
		while (path[i] == '/')
			i++;
		if (path[i] == '\0')
			return (ft_strlcpy(resolved_path, "/", 2), resolved_path);
	}
	if (path[0] == '/')
		ft_strlcpy(tmp, "", 1);
	else if (!getcwd(tmp, sizeof(tmp)))
		return (NULL);
	path_parts = ft_split((char *)path, '/');
	if (!path_parts)
		return (NULL);
	i = 0;
	while (path_parts[i] != NULL)
	{
		if (path_parts[i][0] == '\0' || ft_strcmp(path_parts[i], ".") == 0)
		{
			i++;
			continue ;
		}
		else if (ft_strcmp(path_parts[i], "..") == 0)
		{
			if (tmp[0] != '\0')
			{
				last_slash = strrchr(tmp, '/');
				if (last_slash != NULL)
					*last_slash = '\0';
			}
		}
		else
		{
			if (ft_strlcat(tmp, "/", sizeof(tmp)) >= sizeof(tmp)
				|| ft_strlcat(tmp, path_parts[i], sizeof(tmp)) >= sizeof(tmp))
				return (free_string_array(path_parts), NULL);
			if (lstat(tmp, &statbuf) == -1)
				break ;
		}
		i++;
	}
	free_string_array(path_parts);
	if (ft_strncmp(tmp, "", 1) == 0)
		ft_strlcpy(tmp, "/", 2);
	ft_strlcpy(resolved_path, tmp, PATH_MAX);
	if ((lstat(tmp, &statbuf) == -1))
		return (NULL);
	return (resolved_path);
}
