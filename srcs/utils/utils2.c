#include "../includes/minishell.h"

static int	ft_write_formatted_str(int fd, const char *str)
{
	int	count;
	int	j;
	int	tab_count;
	int	k;

	count = 0;
	j = 0;
	while (str && str[j])
	{
		if (str[j] == '\t')
		{
			tab_count = 0;
			while (str[j++] == '\t')
				tab_count++;
			count += write(fd, "$'\\t", 4);
			k = 1;
			while (k++ < tab_count)
				count += write(fd, "\\t", 2);
			count += write(fd, "'", 1);
		}
		else
			count += write(fd, &str[j++], 1);
	}
	return (count);
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
	while (format && format[i])
	{
		if (format[i] == '%' && format[i + 1] == 's')
		{
			str = va_arg(args, const char *);
			if (str)
				count += ft_write_formatted_str(fd, str);
			i += 2;
		}
		else
			count += write(fd, &format[i++], 1);
	}
	va_end(args);
	return (count);
}

char	*ft_strncpy(char *dest, const char *src, size_t n)
{
	size_t	i;

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
	size_t	i;

	i = 0;
	while (src[i] != '\0')
	{
		dest[i] = src[i];
		i++;
	}
	dest[i] = '\0';
	return (dest);
}
