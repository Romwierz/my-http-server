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

void remove_trailing_newline(char * str)
{
    char trailing_char = str[strlen(str) - 1];
    if (trailing_char == '\n')
    {
        trailing_char = '\0';
        str[strlen(str) - 1] = trailing_char;
    }
}