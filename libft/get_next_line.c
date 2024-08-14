/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcsicsak <dcsicsak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/24 17:34:56 by csicsi            #+#    #+#             */
/*   Updated: 2024/08/11 11:44:23 by dcsicsak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static char	*parse_input(int fd, char *str)
{
	char	*buff;
	ssize_t	bytes;
	char	*temp;

	buff = malloc((BUFFER_SIZE + 1) * sizeof(char));
	if (!buff)
		return (free(str), NULL);
	bytes = read(fd, buff, BUFFER_SIZE);
	while (bytes > 0)
	{
		buff[bytes] = '\0';
		temp = gnl_strjoin(str, buff);
		if (!temp)
			return (free(buff), free(str), NULL);
		free(str);
		str = temp;
		if (gnl_strchr(str, '\n'))
			break ;
		bytes = read(fd, buff, BUFFER_SIZE);
	}
	free(buff);
	if (bytes == -1)
		return (free(str), NULL);
	return (str);
}

static char	*extract_line(char *str)
{
	size_t	len;
	char	*line;

	if (!str)
		return (NULL);
	len = 0;
	while (str[len] && str[len] != '\n')
		len++;
	if (str[len] == '\n')
		len++;
	if (len == 0)
		return (NULL);
	line = malloc((len + 1) * sizeof(char));
	if (!line)
		return (NULL);
	gnl_memcpy(line, str, len);
	line[len] = '\0';
	return (line);
}

char	*get_next_line(int fd)
{
	static char	*str;
	char		*line;
	char		*temp;

	if (fd < 0 || BUFFER_SIZE <= 0)
		return (free(str), NULL);
	if (!gnl_strchr(str, '\n'))
		str = parse_input(fd, str);
	if (!str || *str == '\0')
		return (free(str), str = NULL, NULL);
	line = extract_line(str);
	if (!line || !*line)
		return (free(line), free(str), line = NULL, str = NULL, NULL);
	temp = gnl_strjoin(NULL, str + gnl_strlen(line));
	if (!temp || !str)
		return (free(line), free(str), line = NULL, str = NULL, NULL);
	free(str);
	str = temp;
	return (line);
}
