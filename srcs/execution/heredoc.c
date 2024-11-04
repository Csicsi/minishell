#include "../includes/minishell.h"

void	handle_heredoc(t_cmd *cmd_list, t_data *data)
{
	t_cmd	*current;
	int		fd;
	bool	has_heredoc;

	has_heredoc = false;
	current = cmd_list;
	while (current)
	{
		if (current->is_heredoc)
		{
			has_heredoc = true;
			fd = open_heredoc_file(current);
			if (fd == -1)
				return ;
			read_and_write_heredoc(current, data, fd);
			close(fd);
		}
		current = current->next;
	}
}
