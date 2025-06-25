#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include "request.h"
#include "server.h"
#include "socket_comm.h"
#include "messages.h"
#include "utils.h"

char file_content_buf[1024];
long bytes_in_file;

static int parse_http_request(char *request)
{
    char req_line[30] = { 0 };
    char method[10] = { 0 };
    char uri[10] = { 0 };
    char version[10] = { 0 };
    
    char *next_element;
    int element_size = 0;
    int element_cnt = 0;
    bool req_line_end = false;
    
    for (size_t i = 0; i < strlen(request); i++)
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
                memccpy(method, next_element, ' ', element_size);
                break;
            case 2:
                memccpy(uri, next_element, ' ', element_size);
                break;
            case 3:
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
        return 400;
    
    FILE *fp;
    fp = fopen("www/index.html", "r");

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

    return 200;
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
    case 404:
        socket_transmit(sockfd, HTTP_STATUS_404);
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
