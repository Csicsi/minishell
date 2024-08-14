/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strmapi.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcsicsak <dcsicsak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/05 13:34:30 by dcsicsak          #+#    #+#             */
/*   Updated: 2024/04/05 16:34:04 by dcsicsak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strmapi(char const *s, char (*f)(unsigned int, char))
{
	char	*new;
	int		len;
	int		index;

	len = 0;
	index = 0;
	if (!s || !f)
		return (NULL);
	while (s[len] != '\0')
		len++;
	new = (char *)malloc((len + 1) * sizeof(char));
	if (new == NULL)
		return (NULL);
	while (index < len)
	{
		new[index] = f(index, s[index]);
		index++;
	}
	new[len] = '\0';
	return (new);
}
