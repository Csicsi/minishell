/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_bonus.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: csicsi <csicsi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/12 13:25:43 by dcsicsak          #+#    #+#             */
/*   Updated: 2024/11/12 16:10:55 by csicsi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell_bonus.h"

void	handle_heredoc(t_cmd *cmd_list, t_data *data)
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
				return ;
			read_and_write_heredoc(current, data, fd);
			close(fd);
		}
		current = current->next;
	}
}
