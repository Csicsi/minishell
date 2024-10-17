
#include "../includes/minishell.h"

/**
 * @brief Handles the `pwd` builtin command.
 *
 * This function retrieves and prints the current working directory.
 * If there are too many arguments or the directory cannot be retrieved,
 * it outputs an appropriate error message.
 *
 * @param cmd The command structure containing arguments.
 * @return int Returns 0 on success, or 1 if an error occurs.
 */
int	builtin_pwd(void)
{
	char	cwd[PATH_MAX];

	// Attempt to retrieve the current working directory.
	if (getcwd(cwd, sizeof(cwd)) == NULL)
	{
		// Print an error message if getcwd fails.
		perror(": pwd");
		return (1);
	}

	// Print the current working directory.
	printf("%s\n", cwd);

	return (0);
}
