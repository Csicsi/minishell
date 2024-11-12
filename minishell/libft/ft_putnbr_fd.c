/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putnbr_fd.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcsicsak <dcsicsak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/05 14:31:25 by dcsicsak          #+#    #+#             */
/*   Updated: 2024/04/08 14:02:05 by dcsicsak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_putnbr_fd(int nb, int fd)
{
	int				i;
	int				rev_digits[100];
	unsigned int	nbu;

	if (nb == 0)
	{
		ft_putchar_fd('0', fd);
		return ;
	}
	nbu = nb;
	if (nb < 0)
	{
		ft_putchar_fd('-', fd);
		nbu = -nb;
	}
	i = 0;
	while (nbu > 0)
	{
		rev_digits[i] = nbu % 10;
		nbu /= 10;
		i++;
	}
	while (i > 0)
		ft_putchar_fd(rev_digits[--i] + '0', fd);
}
