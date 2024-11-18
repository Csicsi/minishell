/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_bonus.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcsicsak <dcsicsak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/12 13:25:43 by dcsicsak          #+#    #+#             */
/*   Updated: 2024/11/18 13:10:55 by dcsicsak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell_bonus.h"

static int	read_and_skip_heredoc(t_cmd *cmd, int index)
{
	char	*line;
	int		len;

	len = ft_strlen(cmd->heredoc_delim[index]);
	while (1)
	{
		line = readline("> ");
		if (!line)
		{
			ft_fprintf(2, ": warning: here-document delimited by");
			ft_fprintf(2, " end-of-file (wanted `%s')\n",
				cmd->heredoc_delim[index]);
			break ;
		}
		if (ft_strncmp(line, cmd->heredoc_delim[index], len) == 0
			&& line[len] == '\0')
		{
			free(line);
			break ;
		}
		free(line);
	}
	return (0);
}

static int	read_and_write_heredoc(t_cmd *cmd, t_data *data, int fd, int index)
{
	char	*line;
	int		len;

	len = ft_strlen(cmd->heredoc_delim[index]);
	while (1)
	{
		if (handle_signal_interrupt(cmd))
			return (1);
		line = readline("> ");
		if (!line)
		{
			ft_fprintf(2, ": warning: here-document delimited by");
			ft_fprintf(2, " end-of-file (wanted `%s')\n",
				cmd->heredoc_delim[index]);
			break ;
		}
		if (ft_strncmp(line, cmd->heredoc_delim[index], len) == 0
			&& line[len] == '\0')
		{
			free(line);
			break ;
		}
		write_heredoc_line(fd, line, data);
	}
	return (0);
}

int	handle_single_heredoc(t_cmd *current, t_data *data, int heredoc_index)
{
	int	fd;

	if (heredoc_index == current->heredoc_count - 1)
	{
		fd = open_heredoc_file(current);
		if (fd == -1)
			return (1);
		if (read_and_write_heredoc(current, data, fd, heredoc_index) == 1)
			return (1);
		close(fd);
	}
	else
	{
		if (read_and_skip_heredoc(current, heredoc_index) == 1)
			return (1);
	}
	return (0);
}

int	handle_heredoc(t_cmd *cmd_list, t_data *data)
{
	t_cmd	*current;
	int		heredoc_index;

	current = cmd_list;
	while (current)
	{
		if (current->is_heredoc)
		{
			heredoc_index = 0;
			while (heredoc_index < current->heredoc_count)
			{
				if (handle_single_heredoc(current, data, heredoc_index) == 1)
					return (1);
				heredoc_index++;
			}
		}
		current = current->next;
	}
	return (0);
}
