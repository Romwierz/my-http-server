#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>

#define PRINT_USAGE() fprintf(stderr, "Usage: %s [-p port]\n", argv[0])
#define handle_error(msg) do { perror(msg); exit(EXIT_FAILURE); } while (0)

#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define SERVER_ROOT "www"

void parse_args(int argc, char *argv[]);
void remove_trailing_newline(char *str, size_t max_len);
char *remove_leading_whitespaces(char *str);
bool is_file(const char *path);
size_t get_file_size(const char *path);
bool is_within_root(const char *path);
void convert_uri_to_path(char *uri, char *path);

#endif