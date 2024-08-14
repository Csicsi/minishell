/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_substr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcsicsak <dcsicsak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/05 10:49:39 by dcsicsak          #+#    #+#             */
/*   Updated: 2024/04/09 12:19:34 by dcsicsak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static size_t	ft_length(char const *s, unsigned int start, size_t len)
{
	size_t	s_len;

	if (!s)
		return (0);
	s_len = ft_strlen(s);
	if (len > s_len - start)
		len = s_len - start;
	return (len);
}

char	*ft_substr(char const *s, unsigned int start, size_t len)
{
	size_t		index;
	char		*dest;

	index = 0;
	if (!s || start >= ft_strlen(s))
	{
		dest = (char *)malloc(1 * sizeof(char));
		if (!dest)
			return (NULL);
		dest[0] = '\0';
		return (dest);
	}
	while (index < start)
		index++;
	len = ft_length(s, start, len);
	dest = malloc((len + 1) * sizeof(char));
	if (dest == NULL)
		return (NULL);
	while (index < (start + len) && s[index] != '\0')
	{
		dest[index - start] = s[index];
		index++;
	}
	dest[index - start] = '\0';
	return (dest);
}
