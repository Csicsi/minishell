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

void	free_array_of_strs(char **tab)
{
	int	i;

	i = 0;
	while (tab[i] != NULL)
	{
		free(tab[i]);
		i++;
	}
	free(tab);
}
/*
char	*ft_getenv_path(char **envp)
{
	int		i;
	char	*matchpath;

	matchpath = "PATH=";
	i = 0;
	while (envp[i] != NULL)
	{
		if (ft_strncmp(envp[i], matchpath, 5) == 0)
			return (envp[i] + 5);
		i++;
	}
	return (ft_putstr_fd("Path not found. Command not valid.\n", 2), NULL);
}
*/

char	*ft_getenv(char *env_var_name, char **envp)
{
	int	i;
	int	len_match;

	//printf("env_var_name: %s\n", env_var_name);
	//printf("check len: %d\n", (int)(strlen(env_var_name)));

	char *look_for_match = ft_strjoin(env_var_name, "=");
	free(env_var_name);

	//printf("look_for_match: %s\n", look_for_match);
	//printf("check len: %d\n", (int)(strlen(look_for_match)));

	len_match = strlen(look_for_match);
	i = 0;
    //printf("HEEEEEEERE\n");
    //if (envp == NULL)
    //    printf("IS NULL\n");
	while (envp[i] != NULL)
	{
		//printf("envp[%d]: %s\n", i, envp[i]);
		if (ft_strncmp(envp[i], look_for_match, len_match) == 0)
			return (free(look_for_match), envp[i] + len_match);
		i++;
	}
	return (free(look_for_match), NULL);
}