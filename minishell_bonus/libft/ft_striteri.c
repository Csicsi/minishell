/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_striteri.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcsicsak <dcsicsak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/05 13:46:57 by dcsicsak          #+#    #+#             */
/*   Updated: 2024/04/05 13:51:19 by dcsicsak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

void	ft_striteri(char *s, void (*f)(unsigned int, char *))
{
	int	index;

	index = 0;
	if (!s || !f)
	{
		return ;
	}
	while (s[index] != '\0')
	{
		f(index, &s[index]);
		index++;
	}
	return ;
}
