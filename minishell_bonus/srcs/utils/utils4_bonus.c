/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils4_bonus.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: csicsi <csicsi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/12 13:27:00 by dcsicsak          #+#    #+#             */
/*   Updated: 2024/11/12 16:12:23 by csicsi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell_bonus.h"

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

void	free_string_array(char **string_array)
{
	int	i;

	if (!string_array)
		return ;
	i = 0;
	while (string_array[i])
	{
		free(string_array[i]);
		string_array[i] = NULL;
		i++;
	}
	free(string_array);
	string_array = NULL;
}

void	create_new_tokens(t_token **current,
	char **split_words, int *word_index)
{
	t_token	*new_token;
	int		i;

	i = 1;
	while (split_words[i])
	{
		new_token = malloc(sizeof(t_token));
		if (!new_token)
			return ;
		new_token->value = ft_strdup(split_words[i]);
		if (!new_token->value)
			return (free(new_token));
		new_token->old_value = NULL;
		new_token->type = TOKEN_WORD;
		new_token->word = ++(*word_index);
		new_token->is_expanded = false;
		new_token->next = (*current)->next;
		(*current)->next = new_token;
		*current = new_token;
		i++;
	}
	i = 0;
	while (split_words[i])
		free(split_words[i++]);
	free(split_words);
}
