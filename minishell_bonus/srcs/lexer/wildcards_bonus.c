#include "../includes/minishell_bonus.h"

int filename_matches_pattern(const char *filename, const char *pattern)
{
	size_t filename_len = strlen(filename);
	size_t pattern_len = strlen(pattern);
	char *star_pos = strchr(pattern, '*');
	if (!star_pos)
	{
		int result = strcmp(filename, pattern) == 0;
		return result;
	}
	if (star_pos == pattern)
	{
		int result = strcmp(filename + filename_len - (pattern_len - 1), star_pos + 1) == 0;
		return result;
	}
	else if (star_pos == pattern + pattern_len - 1)
	{
		int result = strncmp(filename, pattern, pattern_len - 1) == 0;
		return result;
	}
	else
	{
		size_t prefix_len = star_pos - pattern;
		size_t suffix_len = pattern_len - prefix_len - 1;
		int result = strncmp(filename, pattern, prefix_len) == 0 &&
					 strcmp(filename + filename_len - suffix_len, star_pos + 1) == 0;
		return result;
	}
}
char **get_matching_files(const char *pattern)
{
	DIR				*dir;
	struct dirent	*entry;
	char			**matches;
	int				count;
	dir = opendir(".");
	if (!dir)
		return NULL;
	matches = malloc(sizeof(char *) * 256);
	if (!matches)
	{
		closedir(dir);
		return NULL;
	}
	count = 0;
	while ((entry = readdir(dir)) != NULL)
	{
		if (entry->d_name[0] == '.')
			continue;
		if (filename_matches_pattern(entry->d_name, pattern))
		{
			matches[count] = strdup(entry->d_name);
			if (!matches[count++])
			{
				while (--count >= 0)
					free(matches[count]);
				free(matches);
				closedir(dir);
				return NULL;
			}
		}
	}
	matches[count] = NULL;
	closedir(dir);
	return matches;
}
int expand_wildcard(t_token *token)
{
	char	**matches = get_matching_files(token->value);
	t_token	*new_token;
	t_token	*current;
	int		i;
	if (!matches || !matches[0])
		return (0);
	free(token->value);
	token->value = strdup(matches[0]);
	if (!token->value)
		return (0);
	current = token;
	for (i = 1; matches[i]; i++)
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
		new_token->type = TOKEN_WORD;
		new_token->word = current->word + 1;
		new_token->is_expanded = 1;
		new_token->is_wildcard = 0;
		new_token->next = current->next;
		current->next = new_token;
		current = new_token;
	}
	for (i = 0; matches[i]; i++)
		free(matches[i]);
	free(matches);
	return (1);
}
