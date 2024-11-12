/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_format.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcsicsak <dcsicsak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/15 15:41:11 by dcsicsak          #+#    #+#             */
/*   Updated: 2024/06/22 14:43:13 by dcsicsak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	handle_format(char format, va_list args)
{
	int	len;

	len = 0;
	if (format == 'c')
		len = ft_printf_c(va_arg(args, int));
	else if (format == 's')
		len = ft_printf_s(va_arg(args, char *));
	else if (format == 'p')
		len = ft_printf_p(va_arg(args, void *));
	else if (format == 'u')
		len = ft_printf_u(va_arg(args, unsigned int));
	else if (format == 'x')
		len = ft_printf_xl(va_arg(args, size_t));
	else if (format == 'X')
		len = ft_printf_xc(va_arg(args, size_t));
	else if (format == '%')
		len = ft_printf_per();
	else if (format == 'd' || format == 'i')
		len = ft_printf_di(va_arg(args, int));
	else
		return (-1);
	return (len);
}
