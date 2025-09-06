#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "builtin.h"
#include "utils.h"

void lsh_loop(void);
int lsh_execute_command(char **args);
void check_lsh_lshrc();
void lsh_run_bash_config(const char *filepath);

/*-----------------------------------------------*/
int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    // Load config files, if any
    check_lsh_lshrc();

    // Run command loop
    lsh_loop();

    // Perform any shutdown/cleanup

    return EXIT_SUCCESS;
}
/*-------------------------------------------------*/

/**
 * The function checks the existence of config file lshrc from the system,
 * else it runs the default file from the project.
 */
void check_lsh_lshrc()
{
    const char *home = getenv("HOME");
    if (!home)
    {
        perror("getenv failed");
        return;
    }

    char path[1024];
    snprintf(path, sizeof(path), "%s/.lshrc", home);

    if (access(path, F_OK) == 0)
    {
        lsh_run_bash_config(path);
    }
    else
    {
        lsh_run_bash_config("lshrc");
    }
}

/**
 * The function runs a file in Bash.
 */
void lsh_run_bash_config(const char *filepath)
{
    pid_t pid = fork();

    if (pid == 0)
    {
        // Child process: run the file using bash
        execl("/bin/bash", "bash", filepath, NULL);
        perror("execl failed");
        exit(EXIT_FAILURE);
    }
    else if (pid > 0)
    {
        // Parent process: wait for bash to finish
        int status;
        waitpid(pid, &status, 0);
    }
    else
    {
        perror("fork failed");
    }
}

/**
 * Handling the commands from terminal.
 */
void lsh_loop(void)
{
    char *line;
    char **args;
    int status;

    do
    {
        printf("> ");
        line = lsh_read_line();
        args = lsh_parse_line(line);
        status = lsh_execute_command(args);

        free(line);
        free(args);
    } while (status);
}

int lsh_execute_command(char **args)
{
    if (args[0] == NULL)
    {
        // Empty command
        return 1;
    }

    for (int i = 0; i < lsh_num_builtins(); i++)
    {
        if (strcmp(args[0], builtin_str[i]) == 0)
        {
            return (*builtin_func[i])(args);
        }
    }
    return lsh_launch(args);
}
