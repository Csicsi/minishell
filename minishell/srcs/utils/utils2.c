/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils2.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcsicsak <dcsicsak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/12 13:26:51 by dcsicsak          #+#    #+#             */
/*   Updated: 2024/11/16 07:12:59 by dcsicsak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

char	*ft_strncpy(char *dest, const char *src, size_t n)
{
	size_t	i;

	i = 0;
	while (src[i] && i < n)
	{
		dest[i] = src[i];
		i++;
	}
	while (i < n)
	{
		dest[i] = '\0';
		i++;
	}
	return (dest);
}

char	*ft_strndup(const char *s, size_t n)
{
	char	*dest;
	size_t	len;

	len = ft_strlen(s);
	if (len > n)
		len = n;
	dest = malloc((len + 1) * sizeof(char));
	if (dest == NULL)
		return (NULL);
	ft_strncpy(dest, s, len);
	dest[len] = '\0';
	return (dest);
}

char	*ft_strcpy(char *dest, const char *src)
{
	size_t	i;

	i = 0;
	while (src[i] != '\0')
	{
		dest[i] = src[i];
		i++;
	}
	dest[i] = '\0';
	return (dest);
}

int	check_for_logical_operators(t_data *data)
{
	char	*cursor;

	cursor = data->input;
	while (*cursor)
	{
		if ((*cursor == '&' && *(cursor + 1) == '&')
			|| (*cursor == '|' && *(cursor + 1) == '|'))
		{
			ft_fprintf(2, "Syntax error: unexpected token `%c%c'\n",
				*cursor, *(cursor + 1));
			return (1);
		}
		cursor++;
	}
	return (0);
}

void	mark_error_on_pipe(t_token *tokens)
{
	t_token	*last_pipe;

	last_pipe = tokens;
	while (tokens)
	{
		if (tokens->type == TOKEN_OPERATOR
			&& ft_strcmp(tokens->value, "|") == 0)
			last_pipe = tokens;
		else if (tokens->type == TOKEN_ERROR)
		{
			last_pipe->type = TOKEN_ERROR;
			break ;
		}
		tokens = tokens->next;
	}
}
