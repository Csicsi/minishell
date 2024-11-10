#include "../includes/minishell.h"

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

char	*generate_random_filename(void)
{
	const char		charset[]
		= "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	unsigned char	random_bytes[FILENAME_LENGTH - 1];
	char			*filename;
	size_t			i;

	filename = malloc(FILENAME_LENGTH);
	if (!filename)
	{
		perror("malloc");
		exit(1);
	}
	generate_random_bytes(random_bytes, FILENAME_LENGTH - 1);
	i = 0;
	while (i < FILENAME_LENGTH - 1)
	{
		filename[i] = charset[random_bytes[i] % (sizeof(charset) - 1)];
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

static void	write_heredoc_line(int fd, char *line, t_data *data)
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
		free(expanded_line);
	}
	else
		write(fd, line, ft_strlen(line));
}

void	read_and_write_heredoc(t_cmd *cmd, t_data *data, int fd)
{
	char	*line;
	int		len;

	len = ft_strlen(cmd->heredoc_delim);
	while (1)
	{
		if (isatty(STDIN_FILENO))
			write(STDOUT_FILENO, "> ", 2);
		line = get_next_line(STDIN_FILENO);
		if (!line)
		{
			write(2, "\n", 1);
			ft_fprintf(2, ": warning: here-document at line 25 delimited by end-of-file (wanted `%s')\n", cmd->heredoc_delim);
			free(line);
			break ;
		}
		if (ft_strncmp(line, cmd->heredoc_delim, len) == 0
				&& line[len] == '\n')
		{
			free(line);
			break ;
		}
		write_heredoc_line(fd, line, data);
	}
}
