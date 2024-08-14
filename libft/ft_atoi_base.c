/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atoi_base.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcsicsak <dcsicsak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/11 20:37:48 by csicsi-vm         #+#    #+#             */
/*   Updated: 2024/06/13 18:46:47 by dcsicsak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	get_value(char c, int base)
{
	int	value;

	if (ft_isdigit(c))
		value = c - '0';
	else if (ft_isalpha(c))
		value = ft_tolower(c) - 'a' + 10;
	else
		return (-1);
	if (value >= base)
		return (-1);
	return (value);
}

static const char	*parse_sign_and_prefix(const char *str, int *sign)
{
	if (*str == '-' || *str == '+')
	{
		if (*str == '-')
			*sign = -1;
		str++;
	}
	if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X'))
		str += 2;
	return (str);
}

int	ft_atoi_base(const char *str, int base)
{
	int	result;
	int	sign;
	int	value;

	result = 0;
	sign = 1;
	if (base < 2 || base > 16)
		return (0);
	while (ft_isspace(*str))
		str++;
	str = parse_sign_and_prefix(str, &sign);
	while (*str)
	{
		value = get_value(*str, base);
		if (value == -1)
			break ;
		result = result * base + value;
		str++;
	}
	return (result * sign);
}
