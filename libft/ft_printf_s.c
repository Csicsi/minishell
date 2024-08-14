/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_s.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcsicsak <dcsicsak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/16 15:06:00 by dcsicsak          #+#    #+#             */
/*   Updated: 2024/06/22 14:42:44 by dcsicsak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_printf_s(char *str)
{
	if (str == NULL)
		return (ft_putstr_fd("(null)", 1), 6);
	ft_putstr_fd(str, 1);
	return (ft_strlen(str));
}
