/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strtrim.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcsicsak <dcsicsak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/05 11:14:58 by dcsicsak          #+#    #+#             */
/*   Updated: 2024/04/05 17:06:51 by dcsicsak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	ft_in_set(char c, char const *set)
{
	while (*set)
	{
		if (c == *set++)
		{
			return (1);
		}
	}
	return (0);
}

static char	*ft_strncpy(char *dest, char const *src, size_t n)
{
	unsigned int	index;

	index = 0;
	while (index < n && src[index] != '\0')
	{
		dest[index] = src[index];
		index++;
	}
	while (index < n)
	{
		dest[index] = '\0';
		index++;
	}
	return (dest);
}

char	*ft_strtrim(char const *s1, char const *set)
{
	char	*dest;
	size_t	len;

	len = 0;
	if (!s1 || !set)
	{
		return (NULL);
	}
	while (*s1 && ft_in_set(*s1, set))
	{
		s1++;
	}
	len = ft_strlen(s1);
	while (len && ft_in_set(s1[len - 1], set))
	{
		len--;
	}
	dest = (char *)malloc((len + 1) * sizeof(char));
	if (dest == NULL)
	{
		return (NULL);
	}
	ft_strncpy(dest, s1, len);
	dest[len] = '\0';
	return (dest);
}
