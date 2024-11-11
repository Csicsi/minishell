#include "../includes/minishell.h"

static int	ft_calculate_formatted_str_len(const char *str)
{
	int	total_len;
	int	tab_count;
	int	j;

	total_len = 0;
	j = 0;
	while (str && str[j])
	{
		if (str[j] == '\t')
		{
			tab_count = 0;
			while (str[j] == '\t')
			{
				tab_count++;
				j++;
			}
			j--;
			total_len += 4 + (2 * (tab_count - 1)) + 1;
		}
		else
			total_len++;
		j++;
	}
	return (total_len);
}

static int	ft_write_tabs_to_buffer(char *buffer,
	int i, int *j, const char *str)
{
	int	tab_count;

	tab_count = 0;
	buffer[i++] = '$';
	buffer[i++] = '\'';
	buffer[i++] = '\\';
	buffer[i++] = 't';
	while (str[*j] == '\t')
	{
		tab_count++;
		(*j)++;
	}
	(*j)--;
	while (--tab_count > 0)
	{
		buffer[i++] = '\\';
		buffer[i++] = 't';
	}
	buffer[i++] = '\'';
	return (i);
}

static void	ft_write_formatted_str_to_buffer(char *buffer, const char *str)
{
	int	i;
	int	j;

	i = 0;
	j = 0;
	while (str && str[j])
	{
		if (str[j] == '\t')
			i = ft_write_tabs_to_buffer(buffer, i, &j, str);
		else
			buffer[i++] = str[j];
		j++;
	}
	buffer[i] = '\0';
}

static int	ft_calculate_total_len(const char *format, va_list args)
{
	int			total_len;
	const char	*str;
	int			i;

	total_len = 0;
	i = 0;
	while (format && format[i])
	{
		if (format[i] == '%' && format[i + 1] == 's')
		{
			str = va_arg(args, const char *);
			if (str)
				total_len += ft_calculate_formatted_str_len(str);
			i += 2;
		}
		else
		{
			total_len++;
			i++;
		}
	}
	return (total_len);
}

static void	ft_fill_buffer(char *buffer, const char *format, va_list args)
{
	const char	*str;
	int			i;
	int			pos;

	i = 0;
	pos = 0;
	while (format && format[i])
	{
		if (format[i] == '%' && format[i + 1] == 's')
		{
			str = va_arg(args, const char *);
			if (str)
				ft_write_formatted_str_to_buffer(buffer + pos, str);
			pos += ft_calculate_formatted_str_len(str);
			i += 2;
		}
		else
			buffer[pos++] = format[i++];
	}
	buffer[pos] = '\0';
}

void	ft_fprintf(int fd, const char *format, ...)
{
	va_list		args;
	int			total_len;
	char		*buffer;

	va_start(args, format);
	total_len = ft_calculate_total_len(format, args);
	va_end(args);
	buffer = malloc(total_len + 1);
	if (!buffer)
		return ;
	va_start(args, format);
	ft_fill_buffer(buffer, format, args);
	va_end(args);
	write(fd, buffer, total_len);
	free(buffer);
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
