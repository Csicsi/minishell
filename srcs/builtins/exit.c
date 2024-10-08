
#include "../includes/minishell.h"

/**
 * @brief Checks if a string represents a valid numeric value.
 *
 * This function checks if the provided string is a valid numeric value.
 * It allows an optional leading '+' or '-' sign followed by digits.
 *
 * @param str The string to check.
 * @return int Returns 1 if the string is numeric, 0 otherwise.
 */
int	ft_strisnum(const char *str)
{
	if (!str || !*str)
		return (0);
	// Handle optional leading '+' or '-'
	if (*str == '+' || *str == '-')
		str++;
	if (!*str)  // Ensure the string isn't just a sign
		return (0);
	while (*str)
	{
		if (!isdigit((unsigned char)*str))
			return (0);
		str++;
	}
	return (1);
}

/**
 * @brief Handles the `exit` builtin command.
 *
 * This function exits the shell, optionally taking an exit status
 * from the command arguments. If too many arguments are provided,
 * it returns an error. If the argument is not numeric, it returns
 * a 255 error code. If no argument is provided, it defaults to an
 * exit status of 0.
 *
 * @param cmd The command structure containing arguments.
 * @param exit_flag A flag to signal the main process to exit.
 * @return int The exit status (0-255).
 */
int	builtin_exit(t_command *cmd, t_data *data, bool print_exit)
{

	// Signal to the main process to exit the shell
	data->exit_flag = true;

	// Print exit message
	if (isatty(STDIN_FILENO) && print_exit)
		fprintf(stderr, "exit\n");

	// Handle too many arguments error
	if (cmd->args[1] && cmd->args[2])
	{
		fprintf(stderr, "minishell: exit: too many arguments\n");
		return 1;
	}

	// Handle non-numeric argument error
	if (cmd->args[1] && ft_strisnum(cmd->args[1]) == 0)
	{
		fprintf(stderr, "minishell: exit: %s: numeric argument required\n", cmd->args[1]);
		return 255;
	}

	// Set exit status based on the argument or default to 0
	if (cmd->args[1])
		data->last_exit_status = atoi(cmd->args[1]);

	// Mask exit status to fit within the 0-255 range
	data->last_exit_status &= 255;

	return data->last_exit_status;
}
