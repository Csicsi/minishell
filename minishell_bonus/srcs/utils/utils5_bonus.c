/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils5_bonus.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcsicsak <dcsicsak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/16 12:09:52 by dcsicsak          #+#    #+#             */
/*   Updated: 2024/11/16 12:12:00 by dcsicsak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell_bonus.h"

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
