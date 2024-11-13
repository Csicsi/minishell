/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd_utils3.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krabitsc <krabitsc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/13 10:55:40 by krabitsc          #+#    #+#             */
/*   Updated: 2024/11/13 14:12:33 by krabitsc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

char *ft_realpath(const char *path, char *resolved_path)
{
	char temp[PATH_MAX];
	char **path_parts;
	int i;
	struct stat statbuf;

	if (path == NULL)
		return (NULL);
    if (path[0] == '/')
    {
        int j = 0;
        while (path[j] == '/')
            j++;
        if (path[j] == '\0')
        {
            return (ft_strlcpy(resolved_path, "/", 2), resolved_path);
        }
    }
	if (path[0] == '/')
		ft_strlcpy(temp, "", 1);
	else if (!getcwd(temp, sizeof(temp)))
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
            continue;
        }
		else if (ft_strcmp(path_parts[i], "..") == 0)
		{
			if (temp[0] != '\0')
			{
				char *last_slash = strrchr(temp, '/');
				if (last_slash != NULL)
					*last_slash = '\0';
			}
		}
		else
		{
			if (ft_strlcat(temp, "/", sizeof(temp)) >= sizeof(temp) ||
			ft_strlcat(temp, path_parts[i], sizeof(temp)) >= sizeof(temp))
				return (free_string_array(path_parts), NULL);
			if (lstat(temp, &statbuf) == -1)
                    break ;
		}
		i++;
	}
	free_string_array(path_parts);
	if (ft_strncmp(temp, "", 1) == 0)
		ft_strlcpy(temp, "/", 2);
	ft_strlcpy(resolved_path, temp, PATH_MAX);
	if ((lstat(temp, &statbuf) == -1))
		return (NULL);
	
	return (resolved_path);
}
