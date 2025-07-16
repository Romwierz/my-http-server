#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <sys/stat.h>

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

void remove_trailing_newline(char *str, size_t max_len)
{
    size_t real_len = strlen(str);
    if (real_len > max_len)
        real_len = max_len;

    // if last character of string is '\n' replace it with '\0'
    if (real_len > 0 && str[real_len - 1] == '\n')
        str[real_len - 1] = '\0';
}

char *remove_leading_whitespaces(char *str)
{
  while(isspace((unsigned char)*str)) str++;

  return str;
}

bool is_file(const char *path)
{
    struct stat statbuf;
    if (stat(path, &statbuf) == -1) {
        perror("stat");
        return false;
    }

    if (S_ISREG(statbuf.st_mode))
        return true;
    else
        return false;
}

// prevent accessing server root's parent directory
bool is_within_root(const char *path)
{
    int goto_parent_dir = 0, goto_subdir = 0;

    for (; path[0] != '\0'; path++)
    {
        // parent dir access cannot be higher than subdir access at any moment
        if (goto_parent_dir > goto_subdir)
            return false;
        
        // do not allow spaces in path
        if (isspace(path[0]))
            return false;
                
        // skip redundant slashes
        if (strncmp("//", path, 2) == 0)
            continue;
        
        if (strncmp("/..", path, 3) == 0)
        {
            path += 3;
            if (path[0] == '/' || path[0] == '\0')
                goto_parent_dir++;
        }
        
        if (path[0] == '/' && path[1] != '\0')
            goto_subdir++;
    }

    if (goto_parent_dir > goto_subdir)
            return false;

    return true;
}

void convert_uri_to_path(char *uri, char *path)
{
    // skip redundant slashes
    while (strncmp("//", uri, 2) == 0)
        uri++;

    if (strcmp("/", uri) == 0)
        strcpy(uri, "/index.html");
    
    // append uri to server root
    strcpy(path, SERVER_ROOT);
    // path = (char *)SERVER_ROOT;
    strcat(path, uri);
}