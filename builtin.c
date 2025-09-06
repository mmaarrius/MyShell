#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "builtin.h"

/**
 * Function declarations for builtin shell commands
 */
int lsh_cd(char **args);
int lsh_help(char **args);
int lsh_exit(char **args);

/**
 * List of builtin commands, followed by their corresponding functions.
 */
char *builtin_str[] = {
    "cd",
    "help",
    "exit"};

int (*builtin_func[])(char **) = {
    &lsh_cd,
    &lsh_help,
    &lsh_exit};

int lsh_num_builtins()
{
    return sizeof(builtin_str) / sizeof(char *);
}

/**
 * Builtin function implementations.
 */
int lsh_cd(char **args)
{
    if (args[1] == NULL)
    {
        fprintf(stderr, "expected argument to \"cd\" \n");
    }
    else
    {
        if (chdir(args[1]) != 0)
        {
            perror("cd failed");
        }
    }
    return 1;
}

int lsh_help(char **args)
{
    (void)args;

    printf("Marius Popa's LSH\n");
    printf("Type program names and arguments, and hit enter.\n");
    printf("The following are built in:\n");

    for (int i = 0; i < lsh_num_builtins(); i++)
    {
        printf("  %s\n", builtin_str[i]);
    }

    printf("Use the man command for information on other programs.\n");
    return 1;
}

int lsh_exit(char **args)
{
    (void)args;
    return 0;
}
