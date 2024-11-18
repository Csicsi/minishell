/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcsicsak <dcsicsak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/04 15:01:08 by krabitsc          #+#    #+#             */
/*   Updated: 2024/11/18 12:54:49 by dcsicsak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

volatile size_t	g_signal_value = 0;

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

static void	handle_sigquit(int sig)
{
	g_signal_value = sig;
	write(2, "Quit (core dumped)\n", 20);
	rl_on_new_line();
	rl_replace_line("", 0);
}

static void	handle_sigquit_child(int sig)
{
	(void)sig;
	exit(128 + SIGQUIT);
}

void	setup_signal_handlers(int context)
{
	if (context == 0)
	{
		signal(SIGINT, handle_sigint);
		signal(SIGQUIT, SIG_IGN);
	}
	else if (context == 1)
	{
		signal(SIGINT, handle_sigint_child);
		signal(SIGQUIT, handle_sigquit);
	}
	else if (context == 2)
	{
		signal(SIGINT, handle_sigint_child);
		signal(SIGQUIT, handle_sigquit_child);
	}
}
