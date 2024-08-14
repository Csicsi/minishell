/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcsicsak <dcsicsak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/15 15:16:40 by dcsicsak          #+#    #+#             */
/*   Updated: 2024/06/22 14:42:56 by dcsicsak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_printf(const char *placeholders, ...)
{
	va_list	args;
	size_t	len;

	len = 0;
	va_start(args, placeholders);
	while (*placeholders != '\0')
	{
		if (*placeholders == '%')
		{
			placeholders++;
			if (in_set("cspdiuxX%", *placeholders))
				len += handle_format(*placeholders, args);
			else
				return (-1);
		}
		else
		{
			write(1, placeholders, 1);
			len++;
		}
		placeholders++;
	}
	va_end(args);
	return (len);
}
