#include "../includes/minishell.h"

/**
 * @brief Skips whitespace characters in the input string.
 *
 * This function moves the cursor forward past any space or tab characters.
 *
 * @param cursor A pointer to the current position in the input string.
 * @return char* A pointer to the first non-space character.
 */
char	*skip_spaces(char *cursor)
{
	while (*cursor && isspace(*cursor)) // Continue as long as there are spaces
		cursor++; // Move the cursor to the next character
	return (cursor);
}

/**
 * @brief Extracts a word enclosed in single quotes from the input.
 *
 * This function handles strings enclosed in single quotes and copies the content
 * between the quotes into a token.
 *
 * @param cursor The current position in the input string.
 * @param token Pointer to the token where the extracted word will be stored.
 * @return char* The updated position in the input string after processing the quoted word.
 */
char *extract_single_quoted_word(char *cursor, t_token *token)
{
	char *start = ++cursor; // Move past the opening single quote
	while (*cursor && *cursor != '\'') // Look for the closing single quote
		cursor++;
	token->value = strndup(start, cursor - start); // Copy the content between the quotes
	return (++cursor); // Move past the closing quote
}

/**
 * @brief Expands environment variables (e.g., $HOME) in the input.
 *
 * This function extracts an environment variable name, fetches its value, and appends
 * it to the result string.
 *
 * @param result A pointer to the result string where the variable's value will be appended.
 * @param index A pointer to the current index in the result string.
 * @param cursor The current position in the input string.
 * @return char* The updated position in the input string after processing the variable.
 */
char *expand_env_var(char **result, int *index, char *cursor, int last_exit_status, int len_result)
{
	char *start = cursor + 1; // Skip the dollar sign
	int len = 0;

    // Check if we are dealing with the special case "$?"
    if (*start == '?')
    {
        // Convert last_exit_status to a string
		char	*exit_status_str = ft_itoa(last_exit_status);
        // Allocate and copy the exit status into result
        int exit_status_len = strlen(exit_status_str);
		*result = realloc(*result, (len_result - 2 + exit_status_len + 1)); // Resize to fit the new value
        strcpy(&(*result)[*index], exit_status_str); // Copy the exit status value
        *index += exit_status_len; // Update the index
        free(exit_status_str);
        return (cursor + 2); // Skip the "$?" (which is 2 characters)
    }

	// Measure the length of the environment variable name
	while (isalnum(start[len]) || start[len] == '_')
		len++;

	// Dynamically allocate memory for the variable name
	char *var_name = malloc(len + 1);
	strncpy(var_name, start, len); // Copy the variable name
	var_name[len] = '\0'; // Null-terminate

	// Fetch the environment variable value
	char *env_value = getenv(var_name);
	free(var_name); // Free the variable name buffer

	if (env_value)
	{
		// Reallocate memory for the result if necessary
		int env_len = strlen(env_value);
		*result = realloc(*result, (len_result - (len + 1) + env_len + 1)); // Resize to fit the new value
		strcpy(&(*result)[*index], env_value); // Copy the value
		*index += env_len; // Update the index
	}
    else
    {
		*result = realloc(*result, (len_result - (len + 1) + 1)); // Resize to fit the new value
    }

	return (cursor + len + 1); // Return the updated cursor position: len for the length of the env var + 1 for the $-sign
}

/**
 * @brief Extracts a word enclosed in double quotes from the input.
 *
 * This function handles strings enclosed in double quotes, supporting environment
 * variable expansion (e.g., $VAR) and escape sequences (e.g., \").
 *
 * @param cursor The current position in the input string.
 * @param token Pointer to the token where the extracted word will be stored.
 * @return char* The updated position in the input string after processing the quoted word.
 */
char *extract_double_quoted_word(char *cursor, t_token *token, int last_exit_status)
{
	char *start = cursor + 1; // Skip the opening double quote
	int len = 0;

	// Measure the length of the quoted string
	while (start[len] && start[len] != '"')
	{
		if (start[len] == '\\' && (start[len + 1] == '"' || start[len + 1] == '$' || start[len + 1] == '\\'))
			len++; // Skip escape sequences
		len++;
	}

	// Allocate the exact amount of memory needed for the quoted content
	char *result = malloc(len + 1); // +1 for the null terminator
	result[len] = '\0';

	// Extract the content, handling escape sequences and variable expansions
	int i = 0;
	cursor++; // Move past the opening double quote
	while (*cursor && *cursor != '"')
	{
		if (*cursor == '$' && (*(cursor + 1) != ' ' && *(cursor + 1) != '\0'  && *(cursor + 1) != '"'))
		{
			cursor = expand_env_var(&result, &i, cursor, last_exit_status, len);
		}
		else if (*cursor == '\\')
		{
			cursor++;
			if (*cursor == '$' || *cursor == '"' || *cursor == '\\')
				result[i++] = *cursor; // Handle escape characters
		}
		else
			result[i++] = *cursor; // Copy the character to result
		cursor++;
	}
	result[i] = '\0'; // Null-terminate the result string
	token->value = strdup(result); // Store the result in the token
	free(result); // Free the temporary buffer

	return (++cursor); // Move past the closing double quote
}

/**
 * @brief Checks for and handles operator tokens (e.g., |, &&, >>).
 *
 * This function identifies different operators and assigns the corresponding
 * operator value to the token.
 *
 * @param cursor The current position in the input string.
 * @param token Pointer to the token where the operator will be stored.
 * @return char* The updated position in the input string after processing the operator.
 */
char	*check_operator(char *cursor, t_token *token)
{
	if (*cursor == '>' && *(cursor + 1) == '>')
	{
		token->value = strdup(">>"); // Handle append operator (>>)
		return (cursor + 2); // Move past the operator
	}
	else if (*cursor == '<' && *(cursor + 1) == '<')
	{
		token->value = strdup("<<"); // Handle here document operator (<<)
		return (cursor + 2);
	}
	else if (*cursor == '&' && *(cursor + 1) == '&')
	{
		token->value = strdup("&&"); // Handle logical AND (&&)
		return (cursor + 2);
	}
	else if (*cursor == '|' && *(cursor + 1) == '|')
	{
		token->value = strdup("||"); // Handle logical OR (||)
		return (cursor + 2);
	}
	else if (*cursor == '>' || *cursor == '<' || *cursor == '|' || *cursor == '&')
	{
		token->value = strndup(cursor, 1); // Handle single character operators (|, >, <, &)
		return (cursor + 1); // Move past the operator
	}
	return (cursor);
}

/**
 * @brief Counts the number of tokens in the input string.
 *
 * This function scans the input string, identifying and counting tokens such as words,
 * operators, quoted strings, environment variables, and special characters.
 *
 * @param cursor The input string.
 * @return int The total number of tokens found.
 */
int	count_tokens(char *cursor)
{
	int	token_count = 0; // Initialize token count

	while (*cursor)
	{
		cursor = skip_spaces(cursor); // Skip any whitespace
		if (*cursor == '\0')
			break;
		// Handle operators like |, &, >, <
		if (*cursor == '|' || *cursor == '&' || *cursor == '>' || *cursor == '<')
		{
			if ((*cursor == '>' && *(cursor + 1) == '>') || (*cursor == '<' && *(cursor + 1) == '<') ||
				(*cursor == '&' && *(cursor + 1) == '&') || (*cursor == '|' && *(cursor + 1) == '|'))
				cursor++; // Handle two-character operators
			cursor++;
			token_count++; // Increment token count
			continue;
		}
		// Handle dots (.) and double dots (..)
		if (*cursor == '.' && (*(cursor + 1) == ' ' || *(cursor + 1) == '\0'))
		{
			token_count++;
			cursor++;
			continue;
		}
		else if (*cursor == '.' && *(cursor + 1) == '.' && (*(cursor + 2) == ' ' || *(cursor + 2) == '\0'))
		{
			token_count++;
			cursor += 2;
			continue;
		}
		// Handle quoted strings
		if (*cursor == '"' || *cursor == '\'')
		{
			char quote = *cursor;
			cursor++;
			while (*cursor && *cursor != quote) // Skip until the closing quote
				cursor++;
			if (*cursor == quote)
				cursor++;
			token_count++;
			continue;
		}

		// Handle regular words
		while (*cursor && !isspace(*cursor) && *cursor != '|' && *cursor != '&' &&
				*cursor != '>' && *cursor != '<')
			cursor++;
		token_count++; // Increment token count for each word
	}

	return (token_count); // Return the total token count
}

/**
 * @brief Goes through a word token where a $-sign was found and expands environment variables
 *
 * This function handles strings that were characterized as TOKEN_WORD and goes through them supporting environment
 * variable expansion (e.g., $VAR) and escape sequences (e.g., \").
 * Uses a similar structure to do this as extract_double_quoted_word
 *
 * @param cursor The current position in the word that has is temporarily treated as the TOKEN_WORD (before env var expansion).
 * @return char* The token.value of that particular argument/word after env variable expansion
 */
char	*expand_env_var_in_str(char **ptr_to_cursor, int last_exit_status)
{
	char 	*cursor;
	int		len;

	cursor = *ptr_to_cursor;
	len = strlen(cursor);
	char *result = malloc(len + 1); // +1 for the null terminator
	result[len] = '\0';
	// Extract the content, handling escape sequences and variable expansions
	int		i = 0;
	while (*cursor)
	{
		if (*cursor == '$' && (*(cursor + 1) != ' ' && *(cursor + 1) != '\0'))
		{
			cursor = expand_env_var(&result, &i, cursor, last_exit_status, len);
		}
		else if (*cursor == '\\')
		{
			cursor++;
			if (*cursor == '$' || *cursor == '"' || *cursor == '\\')
				result[i++] = *cursor; // Handle escape characters
		}
		else
			result[i++] = *cursor; // Copy the character to result
		cursor++;
	}
	result[i] = '\0'; // Null-terminate the result string
	char *tmp = strdup(result); // Store the result in the tmp (which will be passed to the token)
	free(*ptr_to_cursor);
	free(result); // Free the temporary buffer

	return (tmp);
}

/**
 * @brief Tokenizes the input string into an array of tokens.
 *
 * This function processes the input string, breaking it into words, operators,
 * environment variables, and other token types.
 *
 * @param input The input string to tokenize.
 * @param tokens_ptr A pointer to the array of tokens.
 * @param token_count The total number of tokens to expect.
 * @return int The number of tokens successfully created.
 */
int	lexer(char *input, t_token **tokens_ptr, int token_count, int last_exit_status)
{
	t_token	*tokens;
	char	*cursor = input;
	int		length;
	int		i = 0;

	tokens = malloc((token_count + 1) * sizeof(t_token)); // Allocate memory for tokens
	if (!tokens)
		return (-1); // Return error if memory allocation fails

	while (*cursor != '\0')
	{
		cursor = skip_spaces(cursor); // Skip any leading spaces

		if (i >= token_count)
			break;
		// Handle double-quoted strings
		if (*cursor == '"')
		{
			tokens[i].type = TOKEN_WORD;
			cursor = extract_double_quoted_word(cursor, &tokens[i], last_exit_status);
			i++;
			continue;
		}
		// Handle single-quoted strings
		if (*cursor == '\'')
		{
			tokens[i].type = TOKEN_WORD;
			cursor = extract_single_quoted_word(cursor, &tokens[i]);
			i++;
			continue;
		}
		// Handle operators
		if (*cursor == '>' || *cursor == '<' || *cursor == '|' || *cursor == '&')
		{
			tokens[i].type = TOKEN_OPERATOR;
			cursor = check_operator(cursor, &tokens[i]);
			i++;
			continue;
		}

		// Handle single and double dots (., ..)
		if (*cursor == '.' && (*(cursor + 1) == ' ' || *(cursor + 1) == '\0'))
		{
			tokens[i].type = TOKEN_DOT;
			tokens[i].value = strdup(".");
			cursor++;
			i++;
			continue;
		}
		if (*cursor == '.' && *(cursor + 1) == '.' && (*(cursor + 2) == ' ' || *(cursor + 2) == '\0'))
		{
			tokens[i].type = TOKEN_DOTDOT;
			tokens[i].value = strdup("..");
			cursor += 2;
			i++;
			continue;
		}
		// Handle regular words
		length = 0;
		while (!isspace(*cursor) && *cursor != '|' && *cursor != '&'
			&& *cursor != '>' && *cursor != '<' && *cursor != '.' && *cursor != '\0')
		{
			length++;
			cursor++;
		}
		char *tmp = strndup(cursor - length, length);
		if (ft_strchr(tmp, '$'))
		{
			tmp = expand_env_var_in_str(&tmp, last_exit_status);
		}

		tokens[i].value = tmp; // Copy the word into the token
		tokens[i].type = TOKEN_WORD;
		i++;
	}
	tokens[i].type = TOKEN_END; // Mark the end of tokens
	*tokens_ptr = tokens; // Assign the token array to the pointer
	return (token_count);
}

/**
 * @brief Frees the memory allocated for the token array.
 *
 * This function frees the dynamically allocated memory used by each token's value
 * and the token array itself.
 *
 * @param tokens The array of tokens.
 * @param token_count The number of tokens in the array.
 */
void	free_tokens(t_data *data)
{
	int i;

	for (i = 0; i < data->token_count; i++)
	{
		if (data->tokens[i].value)
			free(data->tokens[i].value); // Free each token's value
	}
	free(data->tokens); // Free the token array
}
