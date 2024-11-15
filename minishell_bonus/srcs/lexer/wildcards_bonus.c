/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wildcards_bonus.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcsicsak <dcsicsak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/15 16:18:34 by dcsicsak          #+#    #+#             */
/*   Updated: 2024/11/15 19:51:15 by dcsicsak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell_bonus.h"

int	match_with_star(const char *filename, const char *pattern)
{
	size_t	filename_len;
	size_t	pattern_len;
	char	*star_pos;
	size_t	prefix_len;
	size_t	suffix_len;

	filename_len = ft_strlen(filename);
	pattern_len = ft_strlen(pattern);
	star_pos = ft_strchr(pattern, '*');
	if (star_pos == pattern)
	{
		return (ft_strcmp(filename + filename_len - (pattern_len - 1),
				star_pos + 1) == 0);
	}
	else if (star_pos == pattern + pattern_len - 1)
		return (ft_strncmp(filename, pattern, pattern_len - 1) == 0);
	else
	{
		prefix_len = star_pos - pattern;
		suffix_len = pattern_len - prefix_len - 1;
		return (strncmp(filename, pattern, prefix_len) == 0
			&& ft_strcmp(filename + filename_len - suffix_len,
				star_pos + 1) == 0);
	}
}

int	filename_matches_pattern(const char *filename, const char *pattern)
{
	char	*star_pos;

	star_pos = ft_strchr(pattern, '*');
	if (!star_pos)
		return (ft_strcmp(filename, pattern) == 0);
	return (match_with_star(filename, pattern));
}

int	add_matching_files(DIR *dir, const char *pattern,
	char **matches, int *count)
{
	struct dirent	*entry;

	while (1)
	{
		entry = readdir(dir);
		if (entry == NULL)
			break ;
		if (entry->d_name[0] == '.')
			continue ;
		if (filename_matches_pattern(entry->d_name, pattern))
		{
			matches[*count] = strdup(entry->d_name);
			if (!matches[*count])
			{
				while (--(*count) >= 0)
					free(matches[*count]);
				return (0);
			}
			(*count)++;
		}
	}
	return (1);
}

char	**get_matching_files(const char *pattern)
{
	DIR				*dir;
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
	if (!add_matching_files(dir, pattern, matches, &count))
	{
		while (--count >= 0)
			free(matches[count]);
		free(matches);
		closedir(dir);
		return (NULL);
	}
	matches[count] = NULL;
	closedir(dir);
	return (matches);
}

int	expand_wildcard(t_token *token)
{
	char	**matches;
	t_token	*current;

	matches = get_matching_files(token->value);
	if (!matches || !matches[0])
		return (free_string_array(matches), 0);
	free(token->value);
	token->value = ft_strdup(matches[0]);
	if (!token->value)
		return (free_string_array(matches), 0);
	current = token;
	if (!create_wildcard_tokens(current, matches))
		return (free_string_array(matches), 0);
	free_string_array(matches);
	return (1);
}
