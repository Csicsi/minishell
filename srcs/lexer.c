
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
int calculate_expanded_length(char *cursor, int last_exit_status, t_data *data)
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
					char *env_value = ft_getenv(var_name, data->env_vars);

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
void expand_env_vars_into_buffer(char *result, char *cursor, int last_exit_status, t_data *data)
{
    int i = 0;
    char *start;

	(void)data;

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
					char *env_value = ft_getenv(var_name, data->env_vars);
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
char *expand_env_var(char *cursor, int last_exit_status, t_data *data)
{
    // First pass: calculate the total length required for the result
    int final_length = calculate_expanded_length(cursor, last_exit_status, data);
	//printf("final_length: %d\n", final_length);

    // Allocate memory for the result string
    char *result = malloc(final_length + 1); // +1 for the null terminator
    if (!result)
        return NULL; // Handle allocation failure

    // Second pass: fill the result buffer
    expand_env_vars_into_buffer(result, cursor, last_exit_status, data);

	//printf("len result:   %d\n", (int)strlen(result));
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
char *extract_double_quoted_word(char *cursor, t_token *token, int last_exit_status, t_data *data)
{
    char *start = cursor + 1; // Skip the opening double quote
    int len = 0;
    char *expanded;

    // Temporarily allocate memory based on the raw content before expanding variables
    while (start[len] && start[len] != '"')
    {
        // Handle escaped characters within double quotes (e.g., \", \$, \\)
        if (start[len] == '\\' && (start[len + 1] == '"' || start[len + 1] == '$' || start[len + 1] == '\\'))
            len++; // Skip the escape character and the next one
        len++;
    }

    // Allocate temporary buffer for the raw string content (before expansion)
    char *temp = malloc(len + 1); // +1 for null-terminator
    if (!temp)
        return NULL;

    int i = 0;
    cursor++; // Move past the opening double quote
    while (*cursor && *cursor != '"')
    {
        if (*cursor == '\\' && (*(cursor + 1) == '"' || *(cursor + 1) == '$' || *(cursor + 1) == '\\'))
        {
            // Handle escaped characters
            temp[i++] = *(++cursor);
        }
        else
        {
            temp[i++] = *cursor;
        }
        cursor++;
    }
    temp[i] = '\0'; // Null-terminate the temporary string

    // Now expand any environment variables in the raw content
    expanded = expand_env_var(temp, last_exit_status, data);
    free(temp); // Free the temporary raw content

    // Assign the expanded string to the token value
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
 * @brief Goes through a word token where a $-sign was found and expands environment variables
 *
 * This function handles strings that were characterized as TOKEN_WORD and goes through them supporting environment
 * variable expansion (e.g., $VAR) and escape sequences (e.g., \").
 * Uses a similar structure to do this as extract_double_quoted_word
 *
 * @param cursor The current position in the word that has is temporarily treated as the TOKEN_WORD (before env var expansion).
 * @return char* The token.value of that particular argument/word after env variable expansion
 */
char *expand_env_var_in_str(char **ptr_to_cursor, int last_exit_status, t_data *data)
{
    char *cursor = *ptr_to_cursor;
    int final_length = calculate_expanded_length(cursor, last_exit_status, data);

    char *result = malloc(final_length + 1);  // Allocate enough memory for the expanded string
    if (!result)
        return NULL; // Handle allocation failure

    expand_env_vars_into_buffer(result, cursor, last_exit_status, data);  // Perform the actual expansion

    free(*ptr_to_cursor);  // Free the original unexpanded string
    *ptr_to_cursor = strdup(result);  // Replace with the expanded string

    free(result);  // Free the temporary buffer
    return *ptr_to_cursor;
}

int count_tokens(t_token *tokens)
{
    int count = 0;
    while (tokens)
    {
        count++;
        tokens = tokens->next;
    }
    return count;
}

void join_tokens_in_same_word(t_data *data)
{
    t_token *current = data->tokens;
    t_token *next_token;
    char *new_value;

    while (current && current->next)
    {
        // Check if the current token and the next one belong to the same word
        if (current->word == current->next->word)
        {
            next_token = current->next;

            // Concatenate current->value and next_token->value
            new_value = ft_strjoin(current->value, next_token->value);
            free(current->value); // Free old value of current token
            current->value = new_value; // Assign new concatenated value

            // Link the current token to the token after next_token
            current->next = next_token->next;

            // Free next_token
            free(next_token->value);
            free(next_token);
        }
        else
        {
            current = current->next; // Move to the next token
        }
    }
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
int lexer(char *input, t_data *data, int last_exit_status)
{
    char *cursor = input;
    t_token *current = NULL;
    t_token *new_token;
    int word_index = 0; // Initialize word index
    int in_heredoc = 0; // Flag to track heredoc context

    data->tokens = NULL; // Initialize token list to NULL

    while (*cursor != '\0')
    {
        cursor = skip_spaces(cursor); // Skip initial spaces
        if (*cursor == '\0')
            break; // Handle end of input

        // Allocate a new token
        new_token = malloc(sizeof(t_token));
        if (!new_token)
            return -1; // Return -1 on memory allocation failure
        new_token->next = NULL;
		new_token->type = 0;
		new_token->value = NULL;

        // Set the word index for this token
        new_token->word = word_index;

        // Handle heredoc operator (<<)
        if (*cursor == '<' && *(cursor + 1) == '<')
        {
            new_token->type = TOKEN_OPERATOR;
            cursor = check_operator(cursor, new_token);
            in_heredoc = 1; // Enter heredoc context for the next token
            word_index++;    // Operators create a new word context
        }
        // Handle double-quoted strings
        else if (*cursor == '"')
        {
            cursor = extract_double_quoted_word(cursor, new_token, last_exit_status, data);
            new_token->type = TOKEN_WORD;
        }
        // Handle single-quoted strings
        else if (*cursor == '\'')
        {
            cursor = extract_single_quoted_word(cursor, new_token);
            new_token->type = TOKEN_WORD;
        }
        // Handle other operators
        else if (*cursor == '>' || *cursor == '<' || *cursor == '|' || *cursor == '&')
        {
            new_token->type = TOKEN_OPERATOR;
            cursor = check_operator(cursor, new_token);
            word_index++;    // Operators create a new word context
        }
        // Handle regular words
        else
        {
            char *start = cursor;
            int length = 0;
            while (!isspace(*cursor) && *cursor != '|' && *cursor != '&' &&
                   *cursor != '>' && *cursor != '<' && *cursor != '\0' &&
                   *cursor != '"' && *cursor != '\'') // Stop at any quote
            {
                length++;
                cursor++;
            }
            new_token->value = strndup(start, length);
            if (!new_token->value)
                return -1;

            // Check if there is a $ for environment variable expansion
            if (!in_heredoc && ft_strchr(new_token->value, '$')) // Disable expansion only for the first word after heredoc
            {
                char *expanded = expand_env_var_in_str(&new_token->value, last_exit_status, data);
                if (!expanded)
                {
                    free(new_token->value);
                    free(new_token);
                    return -1;
                }
                new_token->value = expanded;
            }

            new_token->type = TOKEN_WORD;

            // Reset the heredoc flag after the first word is processed
            if (in_heredoc)
                in_heredoc = 0;
        }

        // Append the new token to the linked list
        if (!data->tokens)
            data->tokens = new_token;
        else
            current->next = new_token;
        current = new_token;

        // Increment word index only if followed by a space or operator
        if (isspace(*cursor) || *cursor == '|' || *cursor == '&' || *cursor == '>' || *cursor == '<')
        {
            word_index++; // New word context after a space or operator
        }

        cursor = skip_spaces(cursor); // Skip any spaces after the token
    }

    //remove_null_word_tokens(data);
    join_tokens_in_same_word(data);
    return 0; // Return 0 on success
}

