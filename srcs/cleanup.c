
#include "../includes/minishell.h"

/**
 * @brief Frees a linked list of commands.
 *
 * This function releases all allocated memory in the command list.
 *
 * @param cmd_list The head of the command linked list to free.
 */
static void	free_cmd_list(t_command *cmd_list)
{
    t_command *tmp;

    while (cmd_list)
    {
        tmp = cmd_list;

        // Free each argument in the args array if allocated
        if (cmd_list->args)
        {
            for (int i = 0; cmd_list->args[i]; i++)
            {
                free(cmd_list->args[i]);
                cmd_list->args[i] = NULL;  // Avoid potential double free issues
            }
            free(cmd_list->args);  // Free the args array itself
            cmd_list->args = NULL;
        }

        // Free input redirection file name if allocated
        if (cmd_list->input)
        {
            free(cmd_list->input);
            cmd_list->input = NULL;
        }

        // Free output redirection file name if allocated
        if (cmd_list->output)
        {
            free(cmd_list->output);
            cmd_list->output = NULL;
        }

        // Move to the next command and free the current one
        cmd_list = cmd_list->next;
        free(tmp);  // Free the current command
        tmp = NULL;
    }
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
void free_tokens(t_data *data)
{
    t_token *temp;

    while (data->tokens)
    {
        temp = data->tokens;
        data->tokens = data->tokens->next;

        if (temp->value)
		{
            free(temp->value);  // Ensure value is not NULL before freeing
            temp->value = NULL;  // Set to NULL after freeing to avoid double frees
        }
        free(temp);              // Free the token struct itself
        temp = NULL;             // Set the pointer to NULL after freeing
    }
    data->tokens = NULL;         // Ensure the head of the token list is also NULL
}

/**
 * @brief Frees the duplicated environment variables.
 */
static void free_env_vars(char **env_vars)
{
    if (!env_vars)
        return;

    for (int i = 0; env_vars[i]; i++)
    {
		free(env_vars[i]);
		env_vars[i] = NULL;
	}
    free(env_vars);         // Free the array of environment variable pointers
	env_vars = NULL;
}

/**
 * @brief Frees all dynamically allocated resources in the t_data structure.
 */
void cleanup_data(t_data *data, bool free_env)
{
    // Free command list
    if (data->cmd_list)
    {
        free_cmd_list(data->cmd_list);
        data->cmd_list = NULL;
    }

    // Free token list
    if (data->tokens)
    {
        free_tokens(data);
        data->tokens = NULL;
    }

    // Free duplicated environment variables
    if (data->env_vars && free_env)
    {
        free_env_vars(data->env_vars);
        data->env_vars = NULL;
    }

    // Free input string
    if (data->input)
    {
        free(data->input);
        data->input = NULL;
    }

    // Add more resources to clean up as necessary
}
