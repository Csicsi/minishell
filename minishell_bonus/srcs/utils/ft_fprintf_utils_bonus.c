/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_fprintf_utils_bonus.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: csicsi <csicsi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/12 13:29:13 by dcsicsak          #+#    #+#             */
/*   Updated: 2024/11/12 16:12:10 by csicsi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell_bonus.h"

static int	ft_calculate_formatted_str_len(const char *str)
{
	int	total_len;
	int	tab_count;
	int	j;

	total_len = 0;
	j = 0;
	while (str && str[j])
	{
		if (str[j] == '\t')
		{
			tab_count = 0;
			while (str[j] == '\t')
			{
				tab_count++;
				j++;
			}
			j--;
			total_len += 4 + (2 * (tab_count - 1)) + 1;
		}
		else
			total_len++;
		j++;
	}
	return (total_len);
}

static int	ft_write_tabs_to_buffer(char *buffer,
	int i, int *j, const char *str)
{
	int	tab_count;

	tab_count = 0;
	buffer[i++] = '$';
	buffer[i++] = '\'';
	buffer[i++] = '\\';
	buffer[i++] = 't';
	while (str[*j] == '\t')
	{
		tab_count++;
		(*j)++;
	}
	(*j)--;
	while (--tab_count > 0)
	{
		buffer[i++] = '\\';
		buffer[i++] = 't';
	}
	buffer[i++] = '\'';
	return (i);
}

static void	ft_write_formatted_str_to_buffer(char *buffer, const char *str)
{
	int	i;
	int	j;

	i = 0;
	j = 0;
	while (str && str[j])
	{
		if (str[j] == '\t')
			i = ft_write_tabs_to_buffer(buffer, i, &j, str);
		else
			buffer[i++] = str[j];
		j++;
	}
	buffer[i] = '\0';
}

int	ft_calculate_total_len(const char *format, va_list args)
{
	int			total_len;
	const char	*str;
	int			i;

	total_len = 0;
	i = 0;
	while (format && format[i])
	{
		if (format[i] == '%' && format[i + 1] == 's')
		{
			str = va_arg(args, const char *);
			if (str)
				total_len += ft_calculate_formatted_str_len(str);
			i += 2;
		}
		else
		{
			total_len++;
			i++;
		}
	}
	return (total_len);
}

void	ft_fill_buffer(char *buffer, const char *format, va_list args)
{
	const char	*str;
	int			i;
	int			pos;

	i = 0;
	pos = 0;
	while (format && format[i])
	{
		if (format[i] == '%' && format[i + 1] == 's')
		{
			str = va_arg(args, const char *);
			if (str)
				ft_write_formatted_str_to_buffer(buffer + pos, str);
			pos += ft_calculate_formatted_str_len(str);
			i += 2;
		}
		else
			buffer[pos++] = format[i++];
	}
	buffer[pos] = '\0';
}
