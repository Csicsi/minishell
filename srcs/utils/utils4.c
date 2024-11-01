#include "../includes/minishell.h"

void	wait_for_children(t_exec_context ctx, t_data *data)
{
	int	status;
	int	i;

	i = 0;
	while (i < ctx.num_children)
	{
		waitpid(ctx.child_pids[i], &status, 0);
		if (WIFEXITED(status))
			data->last_exit_status = WEXITSTATUS(status);
		else if (WIFSIGNALED(status))
			data->last_exit_status = 128 + WTERMSIG(status);
		i++;
	}
}

char	*ft_strtrim(const char *str, const char *set)
{
	size_t	start;
	size_t	end;
	char	*trimmed;

	if (!str || !set)
		return (NULL);
	start = 0;
	while (str[start] && ft_strchr(set, str[start]))
		start++;
	end = ft_strlen(str);
	while (end > start && ft_strchr(set, str[end - 1]))
		end--;
	trimmed = (char *)malloc(sizeof(char) * (end - start + 1));
	if (!trimmed)
		return (NULL);
	ft_strlcpy(trimmed, str + start, end - start + 1);
	trimmed[end - start] = '\0';
	return (trimmed);
}
