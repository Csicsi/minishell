/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pwd_bonus.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcsicsak <dcsicsak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/20 11:11:10 by krabitsc          #+#    #+#             */
/*   Updated: 2024/11/18 07:36:52 by dcsicsak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell_bonus.h"

int	builtin_pwd(t_data *data)
{
	char	cwd[PATH_MAX];

	if (getcwd_from_env_var(cwd, sizeof(cwd), data) == NULL)
	{
		perror(": pwd");
		return (1);
	}
	printf("%s\n", cwd);
	return (0);
}
