/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_bonus.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcsicsak <dcsicsak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/12 13:25:43 by dcsicsak          #+#    #+#             */
/*   Updated: 2024/11/18 10:37:42 by dcsicsak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell_bonus.h"

static int	read_and_write_heredoc(t_cmd *cmd, t_data *data, int fd)
{
	char	*line;
	int		len;

	len = ft_strlen(cmd->heredoc_delim);
	while (1)
	{
		if (handle_signal_interrupt(cmd))
			return (1);
		line = readline("> ");
		if (!line)
		{
			ft_fprintf(2, ": warning: here-document at line 25 delimited by");
			ft_fprintf(2, " end-of-file (wanted `%s')\n", cmd->heredoc_delim);
			break ;
		}
		if (ft_strncmp(line, cmd->heredoc_delim, len) == 0 && line[len] == '\0')
		{
			free(line);
			break ;
		}
		write_heredoc_line(fd, line, data);
	}
	return (0);
}

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
