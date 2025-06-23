#ifndef UTILS_H
#define UTILS_H

#define PRINT_USAGE() fprintf(stderr, "Usage: %s [-p port]\n", argv[0])
#define handle_error(msg) do { perror(msg); exit(EXIT_FAILURE); } while (0)

void parse_args(int argc, char *argv[]);
void remove_trailing_newline(char * str, size_t max_len);

#endif