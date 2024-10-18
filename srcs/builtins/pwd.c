#include "../includes/minishell.h"

int	builtin_pwd(void)
{
	char	cwd[PATH_MAX];

	if (getcwd(cwd, sizeof(cwd)) == NULL)
	{
		perror(": pwd");
		return (1);
	}
	printf("%s\n", cwd);
	return (0);
}
