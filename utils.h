#ifndef LSH_UTILS_H
#define LSH_UTILS_H

#define TOK_SIZE 64
#define TOK_DELIM " \n\r\t\a"

int lsh_launch(char **args);
char *lsh_read_line(void);
char **lsh_parse_line(char *line);

#endif // LSH_UTILS_H