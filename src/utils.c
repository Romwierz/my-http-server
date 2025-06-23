#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include "utils.h"

void parse_args(int argc, char *argv[])
{
    if (argc < 2)
    {
        PRINT_USAGE();
        exit(EXIT_FAILURE);
    }

    int opt;
    while ((opt = getopt(argc, argv, "hn:p:")) != -1) {
        switch (opt){
        case 'h':
            PRINT_USAGE();
            exit(EXIT_SUCCESS);
        case 'n':
            printf("%s\n", optarg);
            break;
        case 'p':
            printf("port number: %s\n", optarg);
            break;
        default: /* '?' */
            PRINT_USAGE();
            exit(EXIT_FAILURE);
        }
    }
}

void remove_trailing_newline(char * str, size_t max_len)
{
    size_t real_len = strlen(str);
    if (real_len > max_len)
        real_len = max_len;

    // if last character of string is '\n' replace it with '\0'
    if (real_len > 0 && str[real_len - 1] == '\n')
        str[real_len - 1] = '\0';
}