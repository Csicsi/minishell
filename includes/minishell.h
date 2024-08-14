/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcsicsak <dcsicsak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/14 14:12:22 by dcsicsak          #+#    #+#             */
/*   Updated: 2024/08/14 14:17:56 by dcsicsak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

typedef struct s_command
{
	char				*name;
	char				**args;
	char				*input;
	char				*output;
	int					append_output;
	int					background;
	int					exit_status;
	struct s_command	*next;
	char				**env_vars;
}	t_command;

#endif
