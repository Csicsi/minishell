/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcsicsak <dcsicsak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/03 13:49:41 by dcsicsak          #+#    #+#             */
/*   Updated: 2024/04/09 11:48:16 by dcsicsak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memchr(const void *s, int c, size_t n)
{
	size_t	index;
	char	*temp;

	temp = (char *)s;
	index = 0;
	while (index < n)
	{
		if (temp[index++] == (char)c)
		{
			return ((void *)(temp + index - 1));
		}
	}
	return (NULL);
}
