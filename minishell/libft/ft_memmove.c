/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memmove.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcsicsak <dcsicsak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/03 14:47:18 by dcsicsak          #+#    #+#             */
/*   Updated: 2024/04/08 15:13:37 by dcsicsak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memmove(void *dest, const void *src, size_t n)
{
	size_t				index;
	char unsigned		*d;
	const unsigned char	*s;

	if (!dest && !src)
		return (NULL);
	d = (unsigned char *)dest;
	s = (const unsigned char *)src;
	index = 0;
	if (d < s)
	{
		while (index < n)
		{
			d[index] = s[index];
			index++;
		}
	}
	else
	{
		index = n;
		while (index--)
			d[index] = s[index];
	}
	return (dest);
}
