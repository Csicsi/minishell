
#include "../../includes/minishell.h"

int	ft_strisnum(const char *str)
{
	int					sign;
	unsigned long long	num;
	unsigned long long	max_value;

	if (!str || !*str)
		return (0);

	// Skip leading whitespace
	while (*str && isspace((unsigned char)*str))
		str++;

	// Handle sign
	sign = 1;
	if (*str == '+' || *str == '-')
	{
		if (*str == '-')
			sign = -1;
		str++;
	}

	// Check if the string is empty after skipping sign
	if (!*str || !isdigit((unsigned char)*str))
		return (0);

	// Set max value depending on sign
	if (sign == -1)
		max_value = -(unsigned long long)LLONG_MIN;
	else
		max_value = LLONG_MAX;

	num = 0;
	while (*str && isdigit((unsigned char)*str))
	{
		if (num > (max_value / 10))
			return (0);
		if (num == (max_value / 10) && (unsigned long long)(*str - '0') > (max_value % 10))
			return (0);

		num = num * 10 + (*str - '0');
		str++;
	}

	// Skip trailing whitespace
	while (*str && isspace((unsigned char)*str))
		str++;

	// Ensure no extra characters are left after trailing spaces
	if (*str != '\0')
		return (0);

	return (1);
}

int	builtin_exit(t_command *cmd, t_data *data, bool print_exit)
{
	data->exit_flag = true;
	if (isatty(STDIN_FILENO) && print_exit)
		ft_fprintf(2, "exit\n");
	if (cmd->args[1] && ft_strisnum(cmd->args[1]) == 0)
	{
		ft_fprintf(2, ": exit: %s: numeric argument required\n",
			cmd->args[1]);
		return (2);
	}
	if (cmd->args[1] && cmd->args[2])
	{
		ft_fprintf(2, ": exit: too many arguments\n");
		return (1);
	}
	if (cmd->args[1])
		data->last_exit_status = ft_atoi(cmd->args[1]);
	data->last_exit_status &= 255;
	return (data->last_exit_status);
}
