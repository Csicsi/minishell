/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krabitsc <krabitsc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/04 15:01:08 by krabitsc          #+#    #+#             */
/*   Updated: 2024/11/17 13:45:26 by krabitsc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

volatile size_t g_signal_value = 0;

static void	handle_sigint(int sig)
{
	g_signal_value = sig;
	ioctl(STDIN_FILENO, TIOCSTI, "\n");
	rl_on_new_line();
	rl_replace_line("", 0);
}

static void	handle_sigint_child(int sig)
{
	g_signal_value = sig;
	write(1, "\n", 1);
	rl_on_new_line();
	rl_replace_line("", 0);
}

static void	handle_sigquit_child(int sig)
{
	g_signal_value = sig;
	write(2, "Quit (core dumped)\n", 20);
	rl_on_new_line();
	rl_replace_line("", 0);
}

void	setup_signal_handlers(int context)
{
	struct sigaction	sa_int;
	struct sigaction	sa_quit;

	if (context == 0)
	{
		sa_int.sa_handler = handle_sigint;
		sa_int.sa_flags = SA_RESTART;
		sigemptyset(&sa_int.sa_mask);
		sigaction(SIGINT, &sa_int, NULL);
		sa_quit.sa_handler = SIG_IGN;
		sa_quit.sa_flags = 0;
		sigemptyset(&sa_quit.sa_mask);
		sigaction(SIGQUIT, &sa_quit, NULL);
	}
	else if (context == 1)
	{
		sa_int.sa_handler = handle_sigint_child;
		sa_int.sa_flags = SA_RESTART;
		sigemptyset(&sa_int.sa_mask);
		sigaction(SIGINT, &sa_int, NULL);
		sa_quit.sa_handler = handle_sigquit_child;
		sa_quit.sa_flags = 0;
		sigemptyset(&sa_quit.sa_mask);
		sigaction(SIGQUIT, &sa_quit, NULL);
	}
}