#define _GNU_SOURCE

#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "builtin.h"
#include "utils.h"

/**
 * Launch a program and wait for it to terminate.
 */
int lsh_launch(char **args)
{
    pid_t pid;
    int status;

    pid = fork();
    if (pid == 0)
    {
        // Child process
        if (execvp(args[0], args) == -1)
        {
            // All below this line is executed if the command fails
            perror("error executing command");
        }
        exit(EXIT_FAILURE);
    }
    else if (pid < 0)
    {
        perror("error executing command");
    }
    else
    {
        // Parent process
        do
        {
            // The parent will keep waiting even if the child process is temporarily stopped
            waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1;
}


/**
 * Read a line of input from stdin.
 */
char *lsh_read_line(void) {
    size_t size = 0;
    char *line = NULL;

    if (getline(&line, &size, stdin) == -1)
    {
        if (feof(stdin))
        {
            exit(EXIT_SUCCESS);
        }
        else
        {
            perror("reading line");
            exit(EXIT_FAILURE);
        }
    }

    return line;
}

/**
 * Tokenise the line. We'll assume that the delimiter between words
 * will be just whitespace.
 */
char **lsh_parse_line(char *line) {
    int size = TOK_SIZE;
    char **tokens = malloc(size * sizeof(char *));
    char *token;
    int position = 0;

    if (!tokens)
    {
        fprintf(stderr, "allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, TOK_DELIM);
    while (token)
    {
        tokens[position++] = token;

        if (position >= size)
        {
            size *= 2;
            tokens = realloc(tokens, size * sizeof(char *));

            if (!tokens)
            {
                fprintf(stderr, "allocation error\n");
                exit(EXIT_FAILURE);
            }
        }

        token = strtok(NULL, TOK_DELIM);
    }

    tokens[position] = NULL;
    return tokens;
}