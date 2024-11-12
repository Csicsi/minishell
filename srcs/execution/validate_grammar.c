/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   validate_grammar.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcsicsak <dcsicsak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/12 13:25:53 by dcsicsak          #+#    #+#             */
/*   Updated: 2024/11/12 13:25:54 by dcsicsak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

bool	check_input_error(t_file *input_file, t_cmd *cmd, t_data *data)
{
	if (access(input_file->filename, F_OK) != 0)
	{
		ft_fprintf(2, ": %s: No such file or directory\n",
			input_file->filename);
		cmd->io_error = true;
		data->last_exit_status = 1;
		return (true);
	}
	return (false);
}

bool	check_output_error(t_file *output_file, t_cmd *cmd, t_data *data)
{
	char	*output_dir;

	output_dir = get_directory_from_path(output_file->filename);
	if (output_dir && access(output_dir, F_OK) != 0)
	{
		ft_fprintf(2, ": %s: No such file or directory\n",
			output_file->filename);
		cmd->io_error = true;
		data->last_exit_status = 1;
		free(output_dir);
		return (true);
	}
	free(output_dir);
	return (false);
}
