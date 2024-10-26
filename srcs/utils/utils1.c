#include "../includes/minishell.h"

char	*ft_strjoin_pipex(char *s1, char *s2)
{
	char	*onestr;
	size_t	len1;
	size_t	len2;

	if (s1 == NULL && s2 == NULL)
		return (NULL);
	len1 = 0;
	if (s1 != NULL)
		len1 = ft_strlen(s1);
	len2 = ft_strlen(s2);
	onestr = (char *)malloc(sizeof(char) * (len1 + len2 + 1 + 1));
	if (onestr == NULL)
		return (NULL);
	ft_memcpy(onestr, s1, len1);
	ft_memcpy(onestr + len1, "/", 1);
	ft_memcpy(onestr + len1 + 1, s2, len2 + 1);
	return (onestr);
}

char	*free_null(char *str)
{
	if (str != NULL)
		free(str);
	str = NULL;
	return (NULL);
}

char	*ft_getenv(char *env_var_name, char **envp)
{
	int		i;
	int		len_match;
	char	*look_for_match;

	look_for_match = ft_strjoin(env_var_name, "=");
	free_null(env_var_name);
	len_match = ft_strlen(look_for_match);
	i = 0;
	while (envp && envp[i] != NULL)
	{
		if (ft_strncmp(envp[i], look_for_match, len_match) == 0)
			return (free(look_for_match), envp[i] + len_match);
		i++;
	}
	return (free(look_for_match), NULL);
}

bool	is_all_spaces(char *str)
{
	while (*str)
	{
		if (!ft_isspace(*str))
			return (false);
		str++;
	}
	return (true);
}

char	*skip_spaces(char *cursor)
{
	while (*cursor && ft_isspace(*cursor))
		cursor++;
	return (cursor);
}
