/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_bonus.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcsicsak <dcsicsak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/12 13:25:43 by dcsicsak          #+#    #+#             */
/*   Updated: 2024/11/16 13:33:56 by dcsicsak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell_bonus.h"

int	handle_heredoc(t_cmd *cmd_list, t_data *data)
{
	t_cmd	*current;
	int		fd;

	current = cmd_list;
	while (current)
	{
		if (current->is_heredoc)
		{
			fd = open_heredoc_file(current);
			if (fd == -1)
				return (1);
			if (read_and_write_heredoc(current, data, fd) == 1)
				return (1);
			close(fd);
		}
		current = current->next;
	}
	return (0);
}
