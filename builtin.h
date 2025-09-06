#ifndef BUILTIN_H
#define BUILTIN_H

// Function declarations for builtin shell commands
int lsh_cd(char **args);
int lsh_help(char **args);
int lsh_exit(char **args);

// External declarations for builtin command arrays
extern char *builtin_str[];
extern int (*builtin_func[])(char **);

// Helper function to get number of builtins
int lsh_num_builtins(void);

#endif // BUILTIN_H