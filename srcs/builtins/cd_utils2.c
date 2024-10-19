/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd_utils2.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krabitsc <krabitsc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/19 17:25:02 by krabitsc          #+#    #+#             */
/*   Updated: 2024/10/19 20:05:59 by krabitsc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "../includes/minishell.h"

char *ft_realpath(const char *path, char *resolved_path)
{
    char temp[PATH_MAX];
    char **path_parts;
	char *stack[PATH_MAX];
    int top = 0;
	int	i;

	printf("stack[%d]: %s\n", 0, stack[0]);

    if (path == NULL)
        return NULL;
    // If the path is absolute, start from the root "/"
    if (path[0] == '/')
	{
        strcpy(temp, "/");
        path++;
		 printf("temp (absolute start): %s\n", temp);
    }
	else
	{
        // Otherwise, get the current working directory
        if (getcwd(temp, sizeof(temp)) == NULL)
            return NULL;
		printf("temp (relative start): %s\n", temp);
    }

    // Use ft_split to split the path into path_parts
    path_parts = ft_split((char *)path, '/');
    if (!path_parts)
		return NULL;

	i = 0;
	while (path_parts[i] != NULL)
	{
	    printf("path_parts[%d]: %s\n", i, path_parts[i]);
	    i++;
	}



    // Process each component of the path
	i = 0;
	while (path_parts[i] != NULL) 
	{
        if (strcmp(path_parts[i], ".") == 0)
		{
			
		}
        else if (strcmp(path_parts[i], "..") == 0)
		{
            // Go one level up for ".." if possible
            if (top > 0)
                top--;
            else if (temp[0] != '\0' && strcmp(temp, "/") != 0)
            {
                // If at root, ".." has no effect, otherwise remove the last component
                char *last_slash = strrchr(temp, '/');
                if (last_slash != NULL)
                {
                    *last_slash = '\0'; // Truncate at the last '/'
                    if (strlen(temp) == 0) // If we removed everything, reset to root
                        strcpy(temp, "/");
					printf("temp if ..: %s\n", temp);
                }
            }				
        }
		else
		{
            // Normal directory component, add it to the stack
            stack[top++] = path_parts[i];
        }
		i++;
    }

	printf("stack[%d]: %s\n", 0, stack[0]);

	i = 0;
	while (stack[i] != NULL)
	{
	    printf("stack[%d]: %s\n", i, stack[i]);
	    i++;
	}
	printf("top: %d\n", top);

    // Reconstruct the absolute path
    if (temp[strlen(temp) - 1] != '/')
        ft_strlcat(temp, "/", sizeof(temp)); // Add a trailing slash if not already present
    i = 0;
	while (i < top)
	{
		if (ft_strlcat(temp, "/", sizeof(temp)) >= sizeof(temp) ||
            ft_strlcat(temp, stack[i], sizeof(temp)) >= sizeof(temp))
        {
            // Handle buffer overflow
			free_array_of_strs(path_parts);
            return (NULL);
        }
		i++;
    }

    // Handle special case for root "/"
    if (temp[0] == '\0')
        strcpy(temp, "/");

    // Copy the final resolved path to the output buffer
    if (resolved_path)
        strncpy(resolved_path, temp, PATH_MAX);
    else
		resolved_path = strdup(temp);

    // Free the path_parts array returned by ft_split
	free_array_of_strs(path_parts);

	printf("resolved_path ft_realpath: %s\n", resolved_path);
    return (resolved_path);
}
