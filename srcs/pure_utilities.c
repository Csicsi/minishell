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

char	*ft_getenv(char *env_var_name, char **envp)
{
	int		i;
	int		len_match;
	char	*look_for_match;

	look_for_match = ft_strjoin(env_var_name, "=");
	free(env_var_name);
	len_match = ft_strlen(look_for_match);
	i = 0;
	while (envp[i] != NULL)
	{
		if (ft_strncmp(envp[i], look_for_match, len_match) == 0)
			return (free(look_for_match), envp[i] + len_match);
		i++;
	}
	return (free(look_for_match), NULL);
}

int	ft_fprintf(int fd, const char *format, ...)
{
	va_list		args;
	int			i;
	int			count;
	const char	*str;

	i = 0;
	count = 0;
	va_start(args, format);
	while (format[i])
	{
		if (format[i] == '%' && format[i + 1] == 's')
		{
			str = va_arg(args, const char *);
			if (str)
				count += write(fd, str, ft_strlen(str));
			i += 2;
		}
		else
		{
			count += write(fd, &format[i], 1);
			i++;
		}
	}
	va_end(args);
	return (count);
}

char	*ft_strncpy(char *dest, const char *src, size_t n)
{
	size_t i;

	i = 0;
	while (src[i] && i < n)
	{
		dest[i] = src[i];
		i++;
	}
	while (i < n)
	{
		dest[i] = '\0';
		i++;
	}
	return (dest);
}

char	*ft_strndup(const char *s, size_t n)
{
	char	*dest;
	size_t	len;

	len = ft_strlen(s);
	if (len > n)
		len = n;
	dest = malloc((len + 1) * sizeof(char));
	if (dest == NULL)
		return (NULL);
	ft_strncpy(dest, s, len);
	dest[len] = '\0';
	return (dest);
}

char	*ft_strcpy(char *dest, const char *src)
{
	size_t i;

	i = 0;
	while (src[i] != '\0')
	{
		dest[i] = src[i];
		i++;
	}
	dest[i] = '\0';
	return (dest);
}

void	*ft_realloc(void *ptr, size_t old_size, size_t new_size)
{
	void	*new_ptr;

	if (new_size == 0)
	{
		free(ptr);
		return (NULL);
	}
	if (!ptr)
		return (malloc(new_size));
	new_ptr = malloc(new_size);
	if (!new_ptr)
		return (NULL);
	if (new_size < old_size)
		old_size = new_size;
	ft_memcpy(new_ptr, ptr, old_size);
	free(ptr);
	return (new_ptr);
}
