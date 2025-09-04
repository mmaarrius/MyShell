#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define LINE_SIZE 1024
#define TOK_SIZE 64
#define TOK_DELIM " \n\r\t\a"

void lsh_loop(void);
char* lsh_read_line();
char** lsh_parse_line(char* line);
int lsh_cd(char **args);
int lsh_exit(char **args);
int lsh_help(char **args);
int lsh_execute_command(char** args);
void check_lsh_lshrc();
void lsh_run_bash_config(const char *filepath);

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
    "exit"
};

int (*builtin_func[])(char **) = {
    &lsh_cd,
    &lsh_help,
    &lsh_exit
};

int lsh_num_builtins() {
    return sizeof(builtin_str) / sizeof(char*);
}


/*-----------------------------------------------*/
int main(int args, char **argv) {
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
void check_lsh_lshrc() {
    const char *home = getenv("HOME");
    if (!home) {
        perror("getenv failed");
        return;
    }

    char path[1024];
    snprintf(path, sizeof(path), "%s/.lshrc", home);

    if (access(path, F_OK) == 0) {
        lsh_run_bash_config(path);
    } else {
        lsh_run_bash_config("lshrc");
    }
}

/**
 * The function runs a file in Bash.
 */
void lsh_run_bash_config(const char *filepath) {
    pid_t pid = fork();

    if (pid == 0) {
        // Child process: run the file using bash
        execl("/bin/bash", "bash", filepath, NULL);
        perror("execl failed");
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        // Parent process: wait for bash to finish
        int status;
        waitpid(pid, &status, 0);
    } else {
        perror("fork failed");
    }
}

/**
 * Handling the commands from terminal.
 */
void lsh_loop(void) {
    char *line;
    char **args;
    int status;

    do {
        printf("> ");
        line = lsh_read_line();
        args = lsh_parse_line(line);
        status = lsh_execute_command(args);

        free(line);
        free(args);
    } while(status);
}

/**
 * Read the line
 */
char* lsh_read_line(void) {
    __ssize_t size = 0;
    char* line = NULL;

    if (getline(&line, &size, stdin) == -1) {
        if (feof(stdin)) {
            exit(EXIT_SUCCESS);
        } else {
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
char** lsh_parse_line(char* line) {
    int size = TOK_SIZE;
    char** tokens = malloc(size * sizeof(char*));
    char *token;
    int position = 0;

    if (!tokens) {
        fprintf(stderr, "allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, TOK_DELIM);
    while (token) {
        tokens[position++] = token;

        if (position >= size) {
            size *= 2;
            tokens = realloc(tokens, size * sizeof(char*));

            if (!tokens) {
                fprintf(stderr, "allocation error\n");
                exit(EXIT_FAILURE);
            }
        }

        token = strtok(NULL, TOK_DELIM);
    }

    tokens[position] = NULL;
    return tokens;
}

/**
 * The function performs if the process isn't a shell command
 */
int lsh_launch(char** args) {
    pid_t pid, wpid;
    int status;

    pid = fork();
    if (pid == 0) {
        // Child process
        if (execvp(args[0], args) == -1) {
            // All below this line is executed if the command fails
            perror("error executing command");
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        perror("error executing command");
    } else {
        // Parent process
        do {
            // The parent will keep waiting even if the child process is temporarily stopped
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1;
}

int lsh_execute_command(char **args) {
    if (args[0] == NULL) {
        // Empty command
        return 1;
    }

    for (int i = 0; i < lsh_num_builtins(); i++) {
        if (strcmp(args[0], builtin_str[i]) == 0) {
            return (*builtin_func[i])(args);
        }
    }
    return lsh_launch(args);
}

/**
 * Builtin function implementations.
 */
int lsh_cd(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "expected argument to \"cd\" \n");
    } else {
        if (chdir(args[1]) != 0) {
            perror("cd failed");
        }
    }
    return 1;
}

int lsh_help(char **args) {
    printf("Marius Popa's LSH\n");
    printf("Type program names and arguments, and hit enter.\n");
    printf("The following are built in:\n");

    for (int i = 0; i < lsh_num_builtins(); i++) {
        printf("  %s\n", builtin_str[i]);
    }

    printf("Use the man command for information on other programs.\n");
    return 1;
}

int lsh_exit(char **args) {
    return 0;
}

