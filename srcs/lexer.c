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
 * @brief Calculates the final length of the string after expanding environment variables.
 *
 * This function goes through the input string and calculates how long the result string
 * will be after expanding all the environment variables.
 *
 * @param cursor The current position in the input string.
 * @param last_exit_status The value of the last exit status for handling "$?".
 * @return int The total length required for the result string.
 */
int calculate_expanded_length(char *cursor, int last_exit_status)
{
    int total_length = 0;
    char *start;

    while (*cursor)
    {
        if (*cursor == '$')
        {
            start = cursor + 1;

            if (*start == '?') // Handle special case of $?
            {
                char *exit_status_str = ft_itoa(last_exit_status);
                total_length += strlen(exit_status_str);
                free(exit_status_str);
                cursor += 2; // Skip past "$?"
            }
            else
            {
                int len = 0;
                while (isalnum(start[len]) || start[len] == '_')
                    len++;

                if (len > 0)
                {
                    char *var_name = strndup(start, len);
                    char *env_value = getenv(var_name);
                    free(var_name);

                    if (env_value)
                        total_length += strlen(env_value);
                    cursor += len + 1; // Skip past the variable name and '$'
                }
                else
                {
                    total_length++; // If no valid var, count the '$'
                    cursor++;
                }
            }
        }
        else
        {
            total_length++; // Count normal characters
            cursor++;
        }
    }

    return total_length;
}

/**
 * @brief Expands environment variables in the input string without reallocating.
 *
 * This function processes the input string, expands environment variables, and fills
 * the preallocated result string with the expanded content.
 *
 * @param result The preallocated buffer for the result string.
 * @param cursor The input string with potential environment variables.
 * @param last_exit_status The last exit status for handling "$?".
 */
void expand_env_vars_into_buffer(char *result, char *cursor, int last_exit_status)
{
    int i = 0;
    char *start;

    while (*cursor)
    {
        if (*cursor == '$')
        {
            start = cursor + 1;

            if (*start == '?') // Handle special case of $?
            {
                char *exit_status_str = ft_itoa(last_exit_status);
                strcpy(&result[i], exit_status_str);
                i += strlen(exit_status_str);
                free(exit_status_str);
                cursor += 2; // Skip past "$?"
            }
            else
            {
                int len = 0;
                while (isalnum(start[len]) || start[len] == '_')
                    len++;

                if (len > 0)
                {
                    char *var_name = strndup(start, len);
                    char *env_value = getenv(var_name);
                    free(var_name);

                    if (env_value)
                    {
                        strcpy(&result[i], env_value);
                        i += strlen(env_value);
                    }
                    cursor += len + 1; // Skip past the variable name and '$'
                }
                else
                {
                    result[i++] = *cursor; // No valid variable, copy the '$'
                    cursor++;
                }
            }
        }
        else
        {
            result[i++] = *cursor; // Copy normal characters
            cursor++;
        }
    }

    result[i] = '\0'; // Null-terminate the result string
}

/**
 * @brief Expands environment variables in a string without using realloc.
 *
 * This function first calculates the total length required for the result string,
 * then allocates the necessary memory and expands environment variables into it.
 *
 * @param cursor The input string with potential environment variables.
 * @param last_exit_status The last exit status to expand "$?".
 * @return char* A new string with expanded environment variables.
 */
char *expand_env_var(char *cursor, int last_exit_status)
{
    // First pass: calculate the total length required for the result
    int final_length = calculate_expanded_length(cursor, last_exit_status);

    // Allocate memory for the result string
    char *result = malloc(final_length + 1); // +1 for the null terminator
    if (!result)
        return NULL; // Handle allocation failure

    // Second pass: fill the result buffer
    expand_env_vars_into_buffer(result, cursor, last_exit_status);

    return result; // Return the expanded result
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
    char *expanded;

    // Measure the length of the quoted string, considering expansions
    while (start[len] && start[len] != '"')
    {
        if (start[len] == '\\' && (start[len + 1] == '"' || start[len + 1] == '$' || start[len + 1] == '\\'))
            len++; // Skip escape sequences
        len++;
    }

    // Temporarily allocate memory based on the raw content before expanding variables
    char *temp = malloc(len + 1); // +1 for the null terminator
    int i = 0;

    // Extract the raw content while handling escape sequences
    cursor++; // Move past the opening double quote
    while (*cursor && *cursor != '"')
    {
        if (*cursor == '\\')
        {
            cursor++;
            if (*cursor == '"' || *cursor == '$' || *cursor == '\\')
                temp[i++] = *cursor; // Handle escape characters
        }
        else
        {
            temp[i++] = *cursor;
        }
        cursor++;
    }
    temp[i] = '\0'; // Null-terminate the raw content

    // Now expand any environment variables in the raw content
    expanded = expand_env_var(temp, last_exit_status);
    free(temp); // Free the temporary raw content

    // Allocate the exact amount of memory for the expanded result
    token->value = strdup(expanded);
    free(expanded); // Free the expanded result

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
char *expand_env_var_in_str(char **ptr_to_cursor, int last_exit_status)
{
    char *cursor = *ptr_to_cursor;
    int final_length = calculate_expanded_length(cursor, last_exit_status);

    char *result = malloc(final_length + 1);  // Allocate enough memory for the expanded string
    if (!result)
        return NULL; // Handle allocation failure

    expand_env_vars_into_buffer(result, cursor, last_exit_status);  // Perform the actual expansion

    free(*ptr_to_cursor);  // Free the original unexpanded string
    *ptr_to_cursor = strdup(result);  // Replace with the expanded string

    free(result);  // Free the temporary buffer
    return *ptr_to_cursor;
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
	tokens[i].value = NULL;
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

	if (!data->tokens || !data)
		return ;

	for (i = 0; i < data->token_count; i++)
	{
		if (data->tokens[i].value)
		{
			free(data->tokens[i].value); // Free each token's value
			data->tokens[i].value = NULL;
		}
	}
	free(data->tokens); // Free the token array
	data->tokens = NULL;
}

/**
 * @brief Checks if the first token and the token after a pipe are valid commands.
 *
 * This function loops through the tokens and ensures that the first token and
 * any token immediately following a pipe ('|') is a word (command).
 * If any of these tokens is not a command, it returns an error.
 *
 * @param tokens The array of tokens to check.
 * @return int Returns 0 if all checks pass, otherwise -1 for an error.
 */
int check_commands_in_tokens(t_token *tokens)
{
    int i = 0;

    // Ensure the first token is a valid command
    if (tokens[i].type != TOKEN_WORD)
    {
        fprintf(stderr, "Error: Expected command at the start\n");
        return (-1);
    }

    // Loop through the rest of the tokens
    while (tokens[i].type != TOKEN_END)
    {
        // If a pipe is found, the next token must be a command
        if (tokens[i].type == TOKEN_OPERATOR && strcmp(tokens[i].value, "|") == 0)
        {
            i++; // Move to the next token after the pipe
            if (tokens[i].type != TOKEN_WORD)
            {
                fprintf(stderr, "Error: Expected command after pipe\n");
                return (-1);
            }
        }
        i++; // Move to the next token
    }

    return 0; // All checks passed
}
