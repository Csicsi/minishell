#include "../includes/minishell.h"

void print_parsed_commands(t_cmd *cmd_list)
{
    t_cmd *current_cmd;
    int i;

    current_cmd = cmd_list;
    while (current_cmd)
    {
        printf("Command:\n");

        // Print each argument
        for (i = 0; current_cmd->args[i]; i++)
            printf("  Arg[%d]: %s\n", i, current_cmd->args[i]);

        // Print input redirection if present
        if (current_cmd->input)
            printf("  Input: %s\n", current_cmd->input);

        // Print output redirection if present
        if (current_cmd->output)
            printf("  Output: %s\n", current_cmd->output);

        // Print heredoc if present
        if (current_cmd->is_heredoc)
            printf("  Heredoc Delim: %s\n", current_cmd->heredoc_delim);

        // Print the input/output flag state
        switch (current_cmd->io_flag)
        {
            case IO_NONE_OR_SINGLE:
                printf("  IO Flag: No input/output or single redirection\n");
                break;
            case IO_INPUT_FIRST:
                printf("  IO Flag: Input first\n");
                break;
            case IO_OUTPUT_FIRST:
                printf("  IO Flag: Output first\n");
                break;
            default:
                printf("  IO Flag: Unknown\n");
                break;
        }

        // Move to the next command in the list
        current_cmd = current_cmd->next;
    }
}

t_cmd *initialize_cmd(void)
{
    t_cmd *new_cmd;

    new_cmd = malloc(sizeof(t_cmd));
    if (!new_cmd)
        return (NULL);

    // Initialize all fields of the command structure
    new_cmd->args = NULL;
    new_cmd->input = NULL;
    new_cmd->output = NULL;
    new_cmd->append_output = false;
    new_cmd->exit_status = 0;
    new_cmd->heredoc_delim = NULL;
    new_cmd->is_heredoc = false;
    new_cmd->io_flag = IO_NONE_OR_SINGLE;  // Default flag to no redirection
    new_cmd->next = NULL;

    return new_cmd;
}

t_cmd *parse_tokens(t_data *data)
{
    t_cmd *cmd;
    t_cmd *current_cmd;
    int arg_index;
    int words_count;
    t_token *tmp;
    bool has_input = false;  // Track if input is set
    bool has_output = false;  // Track if output is set

    // Initialize the first command structure
    cmd = initialize_cmd();
    if (!cmd)
        return (NULL);

    current_cmd = cmd;
    arg_index = 0;
    words_count = count_tokens(data->tokens);
    current_cmd->args = ft_calloc(words_count + 1, sizeof(char *));
    if (!current_cmd->args)
        return (NULL);

    tmp = data->tokens;  // Save original token list

    // Parse the tokens
    while (data->tokens)
    {
        if (data->tokens->type == TOKEN_WORD)
        {
            current_cmd->args[arg_index++] = ft_strdup(data->tokens->value);
        }
        else if (data->tokens->type == TOKEN_OPERATOR)
        {
            if (ft_strcmp(data->tokens->value, "<<") == 0)
            {
                // Handle heredoc
                if (!current_cmd->is_heredoc && data->tokens->next)
                {
                    current_cmd->is_heredoc = true;
                    current_cmd->heredoc_delim = ft_strdup(data->tokens->next->value);
                    data->tokens = data->tokens->next;  // Skip over the delimiter
                    has_input = true;  // Set input flag
                }
            }
            else if (ft_strcmp(data->tokens->value, ">") == 0 || ft_strcmp(data->tokens->value, ">>") == 0)
            {
                // Handle output redirection
                if (!current_cmd->output && data->tokens->next)
                {
                    current_cmd->output = ft_strdup(data->tokens->next->value);
                    current_cmd->append_output = (ft_strcmp(data->tokens->value, ">>") == 0);
                    data->tokens = data->tokens->next;  // Skip over the output file
                    has_output = true;  // Set output flag

                    // Update the io_flag based on previous input redirection
                    if (has_input)
                        current_cmd->io_flag = IO_INPUT_FIRST;
                    else
                        current_cmd->io_flag = IO_OUTPUT_FIRST;
                }
            }
            else if (ft_strcmp(data->tokens->value, "<") == 0)
            {
                // Handle input redirection
                if (!current_cmd->input && data->tokens->next)
                {
                    current_cmd->input = ft_strdup(data->tokens->next->value);
                    data->tokens = data->tokens->next;  // Skip over the input file
                    has_input = true;  // Set input flag

                    // Update the io_flag based on previous output redirection
                    if (has_output)
                        current_cmd->io_flag = IO_OUTPUT_FIRST;
                    else
                        current_cmd->io_flag = IO_INPUT_FIRST;
                }
            }
            else if (ft_strcmp(data->tokens->value, "|") == 0)
            {
                // Handle pipe operator, start a new command
                current_cmd->args[arg_index] = NULL;  // Null-terminate the args
                current_cmd->next = initialize_cmd();
                if (!current_cmd->next)
                    return (NULL);  // Handle memory allocation failure

                current_cmd = current_cmd->next;  // Move to the next command
                arg_index = 0;  // Reset arg index for the new command
                words_count = count_tokens(data->tokens);
                current_cmd->args = ft_calloc(words_count + 1, sizeof(char *));
                if (!current_cmd->args)
                    return (NULL);
                has_input = false;  // Reset input flag
                has_output = false;  // Reset output flag
            }
        }

        data->tokens = data->tokens->next;  // Move to the next token
    }

    // Null-terminate the arguments of the last command
    current_cmd->args[arg_index] = NULL;

    data->tokens = tmp;  // Restore the original token list
	//print_parsed_commands(cmd);
    return cmd;
}
