/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memcmp.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcsicsak <dcsicsak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/03 14:37:08 by dcsicsak          #+#    #+#             */
/*   Updated: 2024/04/08 13:04:15 by dcsicsak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_memcmp(const void *s1, const void *s2, size_t n)
{
	const char	*c1;
	const char	*c2;

	c1 = (char *)s1;
	c2 = (char *)s2;
	if (n == 0)
	{
		return (0);
	}
	while ((*c1 == *c2) && n != 1)
	{
		n--;
		c1++;
		c2++;
	}
	return ((unsigned char)*c1 - (unsigned char)*c2);
}
