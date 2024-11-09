#include "../includes/minishell.h"

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
