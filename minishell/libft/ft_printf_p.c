/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_p.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcsicsak <dcsicsak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/16 15:01:28 by dcsicsak          #+#    #+#             */
/*   Updated: 2024/06/22 14:42:38 by dcsicsak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

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

static int	base_rec(unsigned long nbr, char *base, unsigned long len)
{
	unsigned long	num;

	num = 0;
	if (nbr >= len)
		num += base_rec(nbr / len, base, len);
	ft_putchar_fd(base[nbr % len], 1);
	return (num + 1);
}

static int	ft_putnbr_base(unsigned long nbr, char *base)
{
	unsigned long	len;

	len = ft_strlen(base);
	if (!check_base(base))
		return (0);
	return (base_rec(nbr, base, len));
}

static int	ft_printhex(unsigned long nbr)
{
	char	*base;

	base = "0123456789abcdef";
	return (ft_putnbr_base(nbr, base));
}

int	ft_printf_p(void *ptr)
{
	unsigned long	ptr_val;

	if (ptr == NULL)
	{
		ft_putstr_fd("(nil)", 1);
		return (5);
	}
	ptr_val = (unsigned long)ptr;
	ft_putstr_fd("0x", 1);
	if (ptr_val == 0)
	{
		ft_putstr_fd("0", 1);
		return (3);
	}
	else
		return (2 + ft_printhex(ptr_val));
}
