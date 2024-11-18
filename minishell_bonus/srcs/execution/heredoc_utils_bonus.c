/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_utils_bonus.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcsicsak <dcsicsak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/12 13:25:39 by dcsicsak          #+#    #+#             */
/*   Updated: 2024/11/18 07:27:28 by dcsicsak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell_bonus.h"

#define FILENAME_LENGTH 20

static void	generate_random_bytes(unsigned char *buffer, ssize_t length)
{
	int	urandom_fd;

	urandom_fd = open("/dev/urandom", O_RDONLY);
	if (urandom_fd < 0)
	{
		perror("open /dev/urandom");
		exit(1);
	}
	if (read(urandom_fd, buffer, length) != length)
	{
		perror("read /dev/urandom");
		close(urandom_fd);
		exit(1);
	}
	close(urandom_fd);
}

static char	*generate_random_filename(void)
{
	const char		charset[]
		= "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	unsigned char	random_bytes[FILENAME_LENGTH - 2];
	char			*filename;
	size_t			i;

	filename = malloc(FILENAME_LENGTH);
	if (!filename)
	{
		perror("malloc");
		exit(1);
	}
	filename[0] = '.';
	generate_random_bytes(random_bytes, FILENAME_LENGTH - 2);
	i = 0;
	while (i < FILENAME_LENGTH - 2)
	{
		filename[i + 1] = charset[random_bytes[i] % (sizeof(charset) - 1)];
		i++;
	}
	filename[FILENAME_LENGTH - 1] = '\0';
	return (filename);
}

int	open_heredoc_file(t_cmd *cmd)
{
	int	fd;

	cmd->heredoc_tempfile = generate_random_filename();
	if (!cmd->heredoc_tempfile)
	{
		perror("generate_random_filename");
		exit(1);
	}
	fd = open(cmd->heredoc_tempfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd == -1)
	{
		perror("open");
		free(cmd->heredoc_tempfile);
	}
	return (fd);
}

void	write_heredoc_line(int fd, char *line, t_data *data)
{
	char	*expanded_line;

	if (!data->heredoc_single_quote)
	{
		expanded_line = expand_env_var_in_str(&line,
				data->last_exit_status, data);
		if (!expanded_line)
		{
			perror("expand_env_var_in_str");
			free(line);
			return ;
		}
		write(fd, expanded_line, ft_strlen(expanded_line));
		write(fd, "\n", 1);
		free(expanded_line);
	}
	else
	{
		write(fd, line, ft_strlen(line));
		write(fd, "\n", 1);
	}
}

int	handle_signal_interrupt(t_cmd *cmd)
{
	if (g_signal_value == SIGINT)
	{
		if (cmd->heredoc_tempfile)
		{
			unlink(cmd->heredoc_tempfile);
			free(cmd->heredoc_tempfile);
			cmd->heredoc_tempfile = NULL;
		}
		return (1);
	}
	return (0);
}
