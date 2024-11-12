/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strnstr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcsicsak <dcsicsak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/03 13:18:08 by dcsicsak          #+#    #+#             */
/*   Updated: 2024/04/05 17:06:39 by dcsicsak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strnstr(const char *big, const char *little, size_t len)
{
	const char	*start;
	const char	*pattern;
	size_t		little_len;

	little_len = ft_strlen(little);
	if (*little == '\0')
		return ((char *)big);
	while (*big && len >= little_len)
	{
		start = big;
		pattern = little;
		while (*big && *pattern && *big == *pattern)
		{
			big++;
			pattern++;
		}
		if (*pattern == '\0')
			return ((char *)start);
		big = start + 1;
		len--;
	}
	return (0);
}
