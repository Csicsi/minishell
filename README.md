🐚 Minishell – A Tiny UNIX Shell

Minishell is a 42 Vienna project that involves creating a lightweight UNIX shell from scratch. It replicates bash behavior by handling command execution, pipes, redirections, environment variables, signals, built-in commands, and wildcard expansion.

This was a duo project, with clear responsibilities:
🛠 My Contributions:

✅ Tokenization & Parsing – Splitting user input into structured commands.
✅ Execution Flow – Running commands, handling pipes & redirections.
✅ Wildcard Expansion (*) – Implementing pattern matching for filenames.
👩‍💻 My Partner’s Contributions:

✅ Built-in Commands – Implementing echo, cd, pwd, export, unset, env, exit.
✅ Environment Variable Management – Handling export, unset, $VAR expansion.
✅ Signal Handling – Managing Ctrl+C, Ctrl+D, and Ctrl+\.
⚡ Features

    ✅ Interactive Shell – Displays a prompt and processes user input.
    ✅ Tokenization & Parsing – Converts input into a structured format.
    ✅ Execution Management – Runs commands via execve().
    ✅ Pipes (|) – Handles multi-command execution with inter-process communication.
    ✅ Redirections (<, >, >>) – Implements input/output redirection.
    ✅ Heredoc (<<) – Reads input until a delimiter is reached.
    ✅ Wildcard Expansion (*) – Matches filenames dynamically.
    ✅ Built-in Commands – Implements echo, cd, pwd, export, unset, env, exit.
    ✅ Environment Variables ($VAR) – Expands variables dynamically.
    ✅ Signal Handling – Handles Ctrl+C, Ctrl+D, and Ctrl+\.
    ✅ Exit Status Management ($?) – Stores last command’s exit code.
    ✅ Syntax Error Handling – Detects invalid syntax and prints error messages.

🔧 Built-in Commands (Partner’s Contribution)
Command	Description
echo	Prints text to stdout. Supports -n flag.
cd	Changes the current directory.
pwd	Prints the current working directory.
export	Sets environment variables.
unset	Removes environment variables.
env	Displays all environment variables.
exit	Exits the shell with a status code.
📌 Tokenization & Parsing (My Contribution)
🔹 Lexing & Tokenization

    Converts user input into tokens (commands, arguments, operators).
    Recognizes quotes (", '), redirections (<, >), and pipes (|).

🔹 Parsing & AST Construction

    Structures commands into an Abstract Syntax Tree (AST).
    Groups pipes, redirections, and logical operators (&&, ||).

📌 Execution & Wildcard Expansion (My Contribution)
🔹 Command Execution

    Executes built-in commands (handled by my partner).
    Runs external commands using execve().
    Handles pipes using dup2() and redirections (<, >).

🔹 Wildcard Expansion (*)

    Implements filename pattern matching similar to bash.
    Uses opendir() and readdir() to retrieve directory contents.

📌 Environment & Signals (Partner’s Contribution)
🔹 Environment Variables

    Implements export, unset, and $VAR expansion.
    Modifies and retrieves environment variables dynamically.

🔹 Signal Handling

    Manages SIGINT, SIGQUIT, and EOF (Ctrl+D).
    Ensures clean exit and proper signal responses.

🎯 What I Learned

    Lexing & Parsing – Converting raw input into structured commands.
    Process management – Using fork(), execve(), waitpid().
    File descriptors & redirections – Managing pipes via dup2().
    Wildcard Expansion (*) – Implementing pattern matching for filenames.
    Handling pipes & heredocs – Ensuring smooth execution flow.

## Try Minishell in Your Browser 🚀
Click the button below to launch Minishell in your browser:

[![Run on StackBlitz](https://developer.stackblitz.com/img/run_on_stackblitz.svg)](https://stackblitz.com/github/dcsicsak/minishell)
