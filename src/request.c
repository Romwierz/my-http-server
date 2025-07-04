#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#include "request.h"
#include "server.h"
#include "socket_comm.h"
#include "messages.h"
#include "utils.h"

int status_code;

char file_content_buf[1024];
long bytes_in_file;

static void read_file(const char *uri)
{
    // if (uri[0] == '/')
    //     strcpy(uri, "www/index.html");
    
    if (strcmp("/", uri) == 0 || strcmp("/index.html", uri) == 0)
        strcpy(uri, "www/index.html");

    FILE *fp;
    if ((fp = fopen(uri, "r")) == NULL) {
        switch (errno)
        {
        case ENOENT: // No such file or directory
            status_code = 404;
            return;
        case EACCES: // Permission denied
            status_code = 403;
            return;
        default:
            handle_error("fopen");
        }   
    }
    
    /* get the number of bytes */
    fseek(fp, 0L, SEEK_END);
    bytes_in_file = ftell(fp);

    if (bytes_in_file > (long)sizeof(file_content_buf))
        bytes_in_file = (long)sizeof(file_content_buf);

    /* reset the file position indicator to
    the beginning of the file */
    fseek(fp, 0L, SEEK_SET);

    /* copy all the text into the buffer */
    fread(file_content_buf, sizeof(char), bytes_in_file, fp);
    fclose(fp);
}

static int parse_http_request(char *request)
{
    char req_line[150] = { 0 };
    char method[50] = { 0 };
    char uri[50] = { 0 };
    char version[50] = { 0 };

    char *next_element;
    size_t element_size = 0;
    size_t max_element_size = 0;
    int element_cnt = 0;
    bool req_line_end = false;
    
    for (size_t i = 0; i < sizeof(req_line) - 1; i++)
    {
        req_line[i] = request[i];
        if (request[i - 1] == '\r' && request[i] == '\n') {
            break;
        }
    }
    printf("Request line: %s", req_line);

    do
    {   
        if (*request == '\r' || *(request + 1) == '\n')
            req_line_end = true;

        if (!isspace(*request) && element_size == 0)
            next_element = request;
        
        if (!isspace(*request) && *request != '\0')
            element_size++;
        
        if ((isspace(*request) || req_line_end) && element_size > 0) {
            element_cnt++;
            switch (element_cnt)
            {
            case 1:
                max_element_size = sizeof(method) - 1;
                if (element_size > max_element_size)
                    element_size = max_element_size;
                memccpy(method, next_element, ' ', element_size);
                break;
            case 2:
                max_element_size = sizeof(method) - 1;
                // implement 414 Request-URI Too Long
                if (element_size > max_element_size)
                    element_size = max_element_size;
                memccpy(uri, next_element, ' ', element_size);
                break;
            case 3:
                max_element_size = sizeof(method) - 1;
                if (element_size > max_element_size)
                    element_size = max_element_size;
                memccpy(version, next_element, ' ', element_size);
                break;
            default:
                break;
            }
            element_size = 0;
        }

        if (req_line_end)
            break;
    } while (request++);

    printf("method: %s\n", method);
    printf("uri: %s\n", uri);
    printf("version: %s\n", version);

    if (strncmp("GET", method, sizeof("GET")) != 0)
        return status_code = 400;

    read_file(uri);

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
    status_code = 200;

    status_code = parse_http_request(request);

    http_response(status_code, sockfd);
}
