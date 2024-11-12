/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcat.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcsicsak <dcsicsak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/03 13:13:38 by dcsicsak          #+#    #+#             */
/*   Updated: 2024/04/05 16:34:06 by dcsicsak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_strlcat(char *dst, const char *src, size_t size)
{
	unsigned int	index1;
	unsigned int	index2;
	unsigned int	res;

	index1 = 0;
	index2 = 0;
	res = 0;
	while (dst[index1])
		index1++;
	while (src[res])
		res++;
	if (size <= index1)
		res += size;
	else
		res += index1;
	while (src[index2] && (index1 + 1) < size)
	{
		dst[index1] = src[index2];
		index1++;
		index2++;
	}
	dst[index1] = '\0';
	return (res);
}
