/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcsicsak <dcsicsak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/24 17:32:04 by csicsi            #+#    #+#             */
/*   Updated: 2024/07/18 12:29:44 by dcsicsak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	gnl_strlen(char *s)
{
	size_t	len;

	len = 0;
	while (s && s[len])
		len++;
	return (len);
}

char	*gnl_strjoin(char *s1, char *s2)
{
	char	*joined_str;
	int		i;
	int		j;

	if (!s1 && !s2)
		return (NULL);
	joined_str = malloc(gnl_strlen(s1) + gnl_strlen(s2) + 1);
	if (joined_str == NULL)
		return (NULL);
	i = 0;
	j = 0;
	while (s1 && s1[j])
		joined_str[i++] = s1[j++];
	j = 0;
	while (s2 && s2[j])
		joined_str[i++] = s2[j++];
	joined_str[i] = '\0';
	return (joined_str);
}

char	*gnl_strchr(const char *s, int c)
{
	c = (unsigned char)c;
	while (s && *s != '\0')
	{
		if (*s == (char)c)
		{
			return ((char *)s);
		}
		s++;
	}
	if (c == '\0')
		return ((char *)s);
	else
		return (NULL);
}

void	*gnl_memcpy(void *dest, const void *src, size_t n)
{
	size_t				index;
	char unsigned		*d;
	const unsigned char	*s;

	if (!dest && !src)
		return (NULL);
	d = (unsigned char *)dest;
	s = (const unsigned char *)src;
	index = 0;
	while (index < n)
	{
		d[index] = s[index];
		index++;
	}
	return (dest);
}
