/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strrchr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcsicsak <dcsicsak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/03 12:57:51 by dcsicsak          #+#    #+#             */
/*   Updated: 2024/04/09 12:13:02 by dcsicsak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strrchr(const char *s, int c)
{
	char	*last;

	last = NULL;
	c = (unsigned char)c;
	while (*s != '\0')
	{
		if (*s == (char)c)
		{
			last = (char *)s;
		}
		s++;
	}
	if (c == '\0')
		return ((char *)s);
	else
		return (last);
}
