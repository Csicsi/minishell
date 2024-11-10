#ifndef UTILS_H
# define UTILS_H

# include "minishell.h"

typedef struct s_data			t_data;
typedef struct s_token			t_token;
typedef struct s_exec_context	t_exec_context;

//cleanup.c
void	cleanup_data(t_data *data, bool free_env);
void	free_string_array(char **string_array);

//utils1.c
char	*ft_strjoin_pipex(char *s1, char *s2);
char	*free_null(char *str);
char	*ft_getenv(char *env_var_name, char **envp);
bool	is_all_spaces(char *str);
char	*skip_spaces(char *str);

//utils2.c
void	ft_fprintf(int fd, const char *format, ...);
char	*ft_strncpy(char *dest, const char *src, size_t n);
char	*ft_strndup(const char *s, size_t n);
char	*ft_strcpy(char *dest, const char *src);

//utils3.c
void	*ft_realloc(void *ptr, size_t old_size, size_t new_size);
int		ft_isdigit_str(const char *str);
int		check_for_brackets(t_data *data);
bool	check_for_heredoc(t_token *tokens);
void	ft_free(void **ptr);

//utils4.c
void	wait_for_children(t_exec_context ctx, t_data *data);
char	*ft_strtrim(const char *str, const char *set);

#endif
