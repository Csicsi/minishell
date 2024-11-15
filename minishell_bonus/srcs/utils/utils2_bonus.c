/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils2_bonus.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcsicsak <dcsicsak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/12 13:26:51 by dcsicsak          #+#    #+#             */
/*   Updated: 2024/11/15 18:40:42 by dcsicsak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell_bonus.h"

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

int	create_wildcard_tokens(t_token *current, char **matches)
{
	t_token	*new_token;
	int		i;

	i = 1;
	while (matches[i])
	{
		new_token = malloc(sizeof(t_token));
		if (!new_token)
			return (0);
		new_token->value = strdup(matches[i]);
		if (!new_token->value)
			return (free(new_token), 0);
		new_token->old_value = NULL;
		new_token->type = TOKEN_WORD;
		new_token->word = current->word + 1;
		new_token->is_expanded = true;
		new_token->is_wildcard = false;
		new_token->next = current->next;
		current->next = new_token;
		current = new_token;
		i++;
	}
	return (1);
}

int	handle_errors(t_data *data, t_exec_context *ctx, t_cmd *current)
{
	if (data->syntax_error)
	{
		cleanup_data(data, false);
		free(ctx->child_pids);
		return (2);
	}
	if (current->empty_redir)
	{
		cleanup_data(data, false);
		free(ctx->child_pids);
		return (1);
	}
	return (-1);
}
