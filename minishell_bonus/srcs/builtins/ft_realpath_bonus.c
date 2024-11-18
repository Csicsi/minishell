/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_realpath_bonus.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcsicsak <dcsicsak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/13 10:55:40 by krabitsc          #+#    #+#             */
/*   Updated: 2024/11/18 07:36:45 by dcsicsak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell_bonus.h"

int	handle_dot_and_dotdot(char *tmp, char *part)
{
	char	*last_slash;

	if (part[0] == '\0' || ft_strcmp(part, ".") == 0)
		return (1);
	else if (ft_strcmp(part, "..") == 0)
	{
		if (tmp[0] != '\0')
		{
			last_slash = ft_strrchr(tmp, '/');
			if (last_slash != NULL)
				*last_slash = '\0';
		}
		return (1);
	}
	return (0);
}

int	append_path_part(char *tmp, char *part)
{
	struct stat	statbuf;

	if (ft_strlcat(tmp, "/", PATH_MAX) >= PATH_MAX
		|| ft_strlcat(tmp, part, PATH_MAX) >= PATH_MAX)
		return (-1);
	if (lstat(tmp, &statbuf) == -1)
		return (-1);
	return (0);
}

int	process_path_parts(char **path_parts, char *tmp)
{
	int	i;
	int	result;

	i = 0;
	while (path_parts[i] != NULL)
	{
		result = handle_dot_and_dotdot(tmp, path_parts[i]);
		if (result == 1)
		{
			i++;
			continue ;
		}
		if (append_path_part(tmp, path_parts[i]) == -1)
			break ;
		i++;
	}
	return (0);
}

int	handle_root_path(const char *path, char *resolved_path)
{
	int	i;

	if (path[0] != '/')
		return (0);
	i = 0;
	while (path[i] == '/')
		i++;
	if (path[i] == '\0')
	{
		ft_strlcpy(resolved_path, "/", 2);
		return (1);
	}
	return (0);
}

char	*ft_realpath(const char *path, char *resolved_path)
{
	char		tmp[PATH_MAX];
	char		**path_parts;
	struct stat	statbuf;

	if (path == NULL)
		return (NULL);
	if (handle_root_path(path, resolved_path))
		return (resolved_path);
	if (path[0] == '/')
		ft_strlcpy(tmp, "", 1);
	else if (!getcwd(tmp, sizeof(tmp)))
		return (NULL);
	path_parts = ft_split((char *)path, '/');
	if (!path_parts)
		return (NULL);
	if (process_path_parts(path_parts, tmp) == -1)
		return (free_string_array(path_parts), NULL);
	free_string_array(path_parts);
	if (ft_strncmp(tmp, "", 1) == 0)
		ft_strlcpy(tmp, "/", 2);
	ft_strlcpy(resolved_path, tmp, PATH_MAX);
	if (lstat(tmp, &statbuf) == -1)
		return (NULL);
	return (resolved_path);
}
