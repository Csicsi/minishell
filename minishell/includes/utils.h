/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dcsicsak <dcsicsak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/12 13:27:33 by dcsicsak          #+#    #+#             */
/*   Updated: 2024/11/16 07:13:10 by dcsicsak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_H
# define UTILS_H

# include "minishell.h"

typedef struct s_data			t_data;
typedef struct s_token			t_token;
typedef struct s_exec_context	t_exec_context;

//cleanup.c
void	cleanup_data(t_data *data, bool free_env);

//ft_fprintf_utils.c
int		ft_calculate_total_len(const char *format, va_list args);
void	ft_fill_buffer(char *buffer, const char *format, va_list args);

//ft_fprintf.c
void	ft_fprintf(int fd, const char *format, ...);

//utils1.c
char	*ft_strjoin_pipex(char *s1, char *s2);
char	*free_null(char *str);
char	*ft_getenv(char *env_var_name, char **envp);
bool	is_all_spaces(char *str);
char	*skip_spaces(char *str);

//utils2.c
char	*ft_strncpy(char *dest, const char *src, size_t n);
char	*ft_strndup(const char *s, size_t n);
char	*ft_strcpy(char *dest, const char *src);
int		check_for_logical_operators(t_data *data);
void	mark_error_on_pipe(t_token *tokens);

//utils3.c
void	*ft_realloc(void *ptr, size_t old_size, size_t new_size);
int		ft_isdigit_str(const char *str);
int		check_for_brackets(t_data *data);
bool	check_for_heredoc(t_token *tokens);
void	ft_free(void **ptr);

//utils4.c
void	wait_for_children(t_exec_context ctx, t_data *data);
char	*ft_strtrim(const char *str, const char *set);
void	free_string_array(char **string_array);
void	create_new_tokens(t_token **current,
			char **split_words, int *word_index);
char	*ft_strreplace(const char *str, const char *old,
			const char *new);

#endif
