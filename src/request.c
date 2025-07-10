#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <sys/stat.h>

#include "request.h"
#include "server.h"
#include "socket_comm.h"
#include "messages.h"
#include "utils.h"

#define SERVER_ROOT "www"

#define REQ_LINE_SIZE_MAX   150
#define METHOD_SIZE_MAX     25
#define URI_SIZE_MAX        100
#define VERSION_SIZE_MAX    25

enum Http_method_t {
    INVALID, GET, HEAD, POST, PUT, DELETE
};

struct Http_method_map_entry {
    const char *name;
    enum Http_method_t type;
} const http_method_map[] = {
    { "GET", GET },
    { "HEAD", HEAD },
    { "POST", POST },
    { "PUT", PUT },
    { "DELETE", DELETE },
    { 0, INVALID }
};

char file_content_buf[1024];

static int read_file(char *uri)
{
    // skip redundant slashes
    while (strncmp("//", uri, 2) == 0)
        uri++;
    
    if (!is_within_root(uri))
        return 403;
    
    long bytes_in_file;

    memset(file_content_buf, '\0', sizeof(file_content_buf));

    if (strcmp("/", uri) == 0)
        strcpy(uri, "/index.html");

    // append uri to server root
    char path[60] = SERVER_ROOT;
    strcat(path, uri);
    
    FILE *fp;
    if ((fp = fopen(path, "r")) == NULL) {
        switch (errno)
        {
        case ENOENT: // No such file or directory
            return 404;
        case EACCES: // Permission denied
            return 403;
        case ENOTDIR: // Not a directory
            return 404;
        default:
            handle_error("fopen");
        }   
    }
    else if (!is_file(path)) {
        fclose(fp);
        return 403;
    }
    
    // get the number of bytes
    fseek(fp, 0L, SEEK_END);
    bytes_in_file = ftell(fp);

    if (bytes_in_file > (long)sizeof(file_content_buf))
        bytes_in_file = (long)sizeof(file_content_buf);

    // reset the file position indicator to
    // the beginning of the file
    fseek(fp, 0L, SEEK_SET);

    // copy all the text into the buffer
    fread(file_content_buf, sizeof(char), bytes_in_file, fp);
    fclose(fp);

    return 200;
}

// map method name (input string) to enum representation using lookup table
static enum Http_method_t parse_http_method(const char *method)
{   
    const struct Http_method_map_entry *entry;
    for (entry = http_method_map; entry->name; entry++)
        if (strncmp(entry->name, method, strlen(entry->name)) == 0)
            break;
        
    return entry->type;
}

// retrieve http request line elements
static void parse_http_req_line(const char *const req_line, char *method, char *uri, char *version)
{
    char *next_element;
    size_t element_size = 0;
    int element_cnt = 0;
    bool req_line_end = false;

    // each element of request line is separated by space,
    // so the element is written into corresponding string when:
    // - space or null occured
    // - element size is greater than zero
    for (size_t i = 0; i < strlen(req_line); i++)
    {
        if (i > 0 && req_line[i - 1] == '\r' && req_line[i] == '\n')
            req_line_end = true;

        if (!isspace(req_line[i]) && element_size == 0)
            next_element = (char *)&req_line[i];
        
        if (!isspace(req_line[i]))
            element_size++;
        
        if ((isspace(req_line[i]) || req_line_end) && element_size > 0) {
            element_cnt++;
            switch (element_cnt)
            {
            case 1:
                if (element_size > METHOD_SIZE_MAX)
                    element_size = METHOD_SIZE_MAX;
                memccpy(method, next_element, ' ', element_size);
                break;
            case 2:
                // implement 414 req_line-URI Too Long
                if (element_size > URI_SIZE_MAX)
                    element_size = URI_SIZE_MAX;
                memccpy(uri, next_element, ' ', element_size);
                break;
            case 3:
                if (element_size > VERSION_SIZE_MAX)
                    element_size = VERSION_SIZE_MAX;
                memccpy(version, next_element, ' ', element_size);
                break;
            default:
                break;
            }
            element_size = 0;
        }
    }
    
    printf("method: %s\n", method);
    printf("uri: %s\n", uri);
    printf("version: %s\n\n", version);
}

static int parse_http_request(char *request)
{
    int status_code = 200;

    char req_line[REQ_LINE_SIZE_MAX] = { 0 };
    char method[METHOD_SIZE_MAX] = { 0 };
    char uri[URI_SIZE_MAX] = { 0 };
    char version[VERSION_SIZE_MAX] = { 0 };
    enum Http_method_t method_type;
    
    for (size_t i = 0; i < sizeof(req_line) - 1; i++)
    {
        req_line[i] = request[i];
        if (i > 0 && request[i - 1] == '\r' && request[i] == '\n') {
            break;
        }
    }
    printf("Request line: %s", req_line);

    parse_http_req_line(req_line, method, uri, version);

    switch (method_type = parse_http_method(method))
    {
    case GET:
        status_code = read_file(uri);
        break;
    case INVALID:
        status_code = 400;
        break;
    default:
        status_code = 501;
        break;
    }

    return status_code;
}

static void http_response(int status_code, int sockfd)
{
    switch (status_code)
    {
    case 200:
        socket_transmit(sockfd, HTTP_STATUS_200);
        socket_transmit(sockfd, "\r\n");
        socket_transmit(sockfd, file_content_buf);
        break;
    case 400:
        socket_transmit(sockfd, HTTP_STATUS_400);
        break;
    case 403:
        socket_transmit(sockfd, HTTP_STATUS_403);
        break;
    case 404:
        socket_transmit(sockfd, HTTP_STATUS_404);
        break;
    case 413:
        socket_transmit(sockfd, HTTP_STATUS_413);
        break;
    case 414:
        socket_transmit(sockfd, HTTP_STATUS_414);
        break;
    case 501:
        socket_transmit(sockfd, HTTP_STATUS_501);
        break;
    default:
        break;
    }

    printf("HTTP response %d sent to client fd%d\n\n", status_code, sockfd);
}

void handle_request(char *request, int sockfd)
{
    int status_code = parse_http_request(request);

    http_response(status_code, sockfd);
}
