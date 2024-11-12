/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_xc.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcsicsak <dcsicsak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/16 15:04:07 by dcsicsak          #+#    #+#             */
/*   Updated: 2024/06/22 14:42:51 by dcsicsak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static void	ft_putchar(char c)
{
	write(1, &c, 1);
}

static int	check_base(char *base)
{
	int	index;
	int	duplicate[256];

	index = 0;
	while (index < 256)
	{
		duplicate[index] = 0;
		index++;
	}
	index = 0;
	while (base[index])
	{
		if (duplicate[(unsigned char)base[index]])
			return (0);
		if (base[index] == '+' || base[index] == '-')
			return (0);
		duplicate[(unsigned char)base[index]] = 1;
		index++;
	}
	if (index <= 1)
		return (0);
	return (1);
}

static int	base_rec(unsigned int nbr, char *base, unsigned int len)
{
	int	num;

	num = 0;
	if (nbr >= len)
		num += base_rec(nbr / len, base, len);
	ft_putchar(base[nbr % len]);
	return (num + 1);
}

static int	ft_putnbr_base(unsigned int nbr, char *base)
{
	unsigned int	len;

	len = ft_strlen(base);
	if (!check_base(base))
		return (0);
	return (base_rec(nbr, base, len));
}

int	ft_printf_xc(unsigned int nbr)
{
	char	*base;

	base = "0123456789ABCDEF";
	return (ft_putnbr_base(nbr, base));
}
