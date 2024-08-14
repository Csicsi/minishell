/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atof.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcsicsak <dcsicsak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/22 12:48:51 by dcsicsak          #+#    #+#             */
/*   Updated: 2024/06/22 12:56:58 by dcsicsak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static void	skip_whitespace(const char *str, int *index)
{
	while (ft_isspace(str[*index]))
		(*index)++;
}

static int	handle_sign(const char *str, int *index)
{
	int	sign;

	sign = 1;
	if (str[*index] == '-')
	{
		sign = -1;
		(*index)++;
	}
	else if (str[*index] == '+')
	{
		(*index)++;
	}
	return (sign);
}

static double	parse_integer_part(const char *str, int *index)
{
	double	result;

	result = 0.0;
	while (ft_isdigit(str[*index]))
	{
		result = result * 10.0 + (str[*index] - '0');
		(*index)++;
	}
	return (result);
}

static double	parse_fraction_part(const char *str, int *index)
{
	double	fraction;
	int		divisor;

	fraction = 0.0;
	divisor = 1;
	if (str[*index] == '.')
	{
		(*index)++;
		while (ft_isdigit(str[*index]))
		{
			fraction = fraction * 10.0 + (str[*index] - '0');
			divisor *= 10;
			(*index)++;
		}
	}
	return (fraction / divisor);
}

double	ft_atof(const char *str)
{
	double	result;
	int		sign;
	int		i;

	i = 0;
	skip_whitespace(str, &i);
	sign = handle_sign(str, &i);
	result = parse_integer_part(str, &i);
	result += parse_fraction_part(str, &i);
	result *= sign;
	return (result);
}
