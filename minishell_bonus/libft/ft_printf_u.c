/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_u.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcsicsak <dcsicsak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/16 15:02:37 by dcsicsak          #+#    #+#             */
/*   Updated: 2024/06/22 14:42:48 by dcsicsak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_printf_u(unsigned int nbr)
{
	int				i;
	int				rev_digits[100];
	int				digits;

	if (nbr == 0)
		return (ft_putchar_fd('0', 1), 1);
	i = 0;
	while (nbr > 0)
	{
		rev_digits[i++] = nbr % 10;
		nbr /= 10;
	}
	digits = i;
	while (i > 0)
		ft_putchar_fd(rev_digits[--i] + '0', 1);
	return (digits);
}
