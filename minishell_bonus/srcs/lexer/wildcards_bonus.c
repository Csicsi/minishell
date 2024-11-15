/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wildcards_bonus.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcsicsak <dcsicsak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/15 16:18:34 by dcsicsak          #+#    #+#             */
/*   Updated: 2024/11/15 16:24:48 by dcsicsak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell_bonus.h"

int	filename_matches_pattern(const char *filename,
	const char *pattern)
{
	size_t	filename_len;
	size_t	pattern_len;
	size_t	prefix_len;
	size_t	suffix_len;
	char	*star_pos;
	int		result;

	filename_len = ft_strlen(filename);
	pattern_len = ft_strlen(pattern);
	star_pos = ft_strchr(pattern, '*');
	if (!star_pos)
	{
		result = ft_strcmp(filename, pattern) == 0;
		return (result);
	}
	if (star_pos == pattern)
	{
		result = ft_strcmp(filename + filename_len - (pattern_len - 1),
				star_pos + 1) == 0;
		return (result);
	}
	else if (star_pos == pattern + pattern_len - 1)
	{
		result = strncmp(filename, pattern, pattern_len - 1) == 0;
		return (result);
	}
	else
	{
		prefix_len = star_pos - pattern;
		suffix_len = pattern_len - prefix_len - 1;
		result = strncmp(filename, pattern, prefix_len) == 0
			&& ft_strcmp(filename + filename_len - suffix_len, star_pos + 1)
			== 0;
		return (result);
	}
}

char	**get_matching_files(const char *pattern)
{
	DIR				*dir;
	struct dirent	*entry;
	char			**matches;
	int				count;

	dir = opendir(".");
	if (!dir)
		return (NULL);
	matches = malloc(sizeof(char *) * 256);
	if (!matches)
	{
		closedir(dir);
		return (NULL);
	}
	count = 0;
	while ((entry = readdir(dir)) != NULL)
	{
		if (entry->d_name[0] == '.')
			continue ;
		if (filename_matches_pattern(entry->d_name, pattern))
		{
			matches[count] = strdup(entry->d_name);
			if (!matches[count++])
			{
				while (--count >= 0)
					free(matches[count]);
				free(matches);
				closedir(dir);
				return (NULL);
			}
		}
	}
	matches[count] = NULL;
	closedir(dir);
	return (matches);
}

int	expand_wildcard(t_token *token)
{
	char	**matches;
	t_token	*new_token;
	t_token	*current;
	int		i;

	matches = get_matching_files(token->value);
	if (!matches || !matches[0])
		return (0);
	free(token->value);
	token->value = strdup(matches[0]);
	if (!token->value)
		return (0);
	current = token;
	i = 1;
	while (matches[i])
	{
		new_token = malloc(sizeof(t_token));
		if (!new_token)
			return (0);
		new_token->value = strdup(matches[i]);
		if (!new_token->value)
		{
			free(new_token);
			return (0);
		}
		new_token->old_value = NULL;
		new_token->type = TOKEN_WORD;
		new_token->word = current->word + 1;
		new_token->is_expanded = true;
		new_token->is_wildcard = false;
		new_token->next = current->next;
		current->next = new_token;
		current = new_token;
		i++;
	}
	i = 0;
	while (matches[i])
	{
		free(matches[i]);
		i++;
	}
	free(matches);
	return (1);
}
