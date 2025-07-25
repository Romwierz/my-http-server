#include <stdio.h>

#include "response.h"
#include "socket_comm.h"
#include "messages.h"
#include "mime.h"

static const char *get_status_line(int status_code)
{
    const struct http_status_t *http_status;

    for (http_status = http_status_map; http_status->code != 0; http_status++)
        if (http_status->code == status_code)
            break;

    return http_status->line;
}

static void send_headers(struct Header_field_t *headers, int sockfd)
{
    while (headers != NULL) {
        socket_transmit(sockfd, headers->name, strlen(headers->name));
        socket_transmit(sockfd, ": ", strlen(": "));
        socket_transmit(sockfd, headers->value, strlen(headers->value));
        socket_transmit(sockfd, CRLF, strlen(CRLF));
        printf("%s: %s\n", headers->name, headers->value);
        headers = headers->next;
    }
}

static void send_file(const char *filepath, size_t filesize, int sockfd)
{
    if (filepath == NULL || filesize == 0)
        return;
    
    char send_buf[SEND_BUF_SIZE_MAX] = { 0 };
    size_t bytes_to_read = filesize;
    size_t data_size;
    FILE *fp;

    const mime_t mime_type = get_mime_type(filepath);

    if ((fp = fopen(filepath, mime_type.read_mode)) == NULL)
        return;

    // reset the file position indicator to the beginning of the file
    fseek(fp, 0L, SEEK_SET);

    while (bytes_to_read > 0)
    {
        data_size = bytes_to_read > sizeof(send_buf) ? sizeof(send_buf) : bytes_to_read;
        bytes_to_read -= fread(send_buf, sizeof(char), data_size, fp);
        if (socket_transmit(sockfd, send_buf, data_size) != data_size)
            continue; // TODO: handle error
    }
    
    fclose(fp);

    return;
}

void send_http_response(struct Http_response_t *http_resp, int sockfd)
{
    const char *status_line = get_status_line(http_resp->status_code);
    printf("Data sent:\n%s", status_line);

    socket_transmit(sockfd, status_line, strlen(status_line));
    send_headers(http_resp->headers, sockfd);
    socket_transmit(sockfd, CRLF, strlen(CRLF));
    send_file(http_resp->filepath, http_resp->filesize, sockfd);

    printf("\n");
}

void add_response_header(struct Http_response_t *http_resp, const char *name, const char *value)
{
    struct Header_field_t *head = http_resp->headers;
    struct Header_field_t *current = head;
    struct Header_field_t *tail = NULL;
    bool header_found = false;

    // iterate over header list until finding the one with name passed to this function
    while (current != NULL) {
        if (strcmp(current->name, name) == 0) {
            header_found = true;
            // return if value is already contained by existing header
            if (strstr(current->value, value))
                return;
            break;
        }
        tail = current;
        current = current->next;
    }
    
    // if header found append new value, else create new header
    if (header_found) {
        size_t new_length = strlen(current->value) + strlen(", ") + strlen(value) + 1;
        current->value = (char *) realloc(current->value, new_length);
        strcat(current->value, ", ");
        strncat(current->value, value, strlen(value));
    }
    else {
        struct Header_field_t *new_header = (struct Header_field_t *) malloc(sizeof(struct Header_field_t));
        if (new_header == NULL)
            handle_error("malloc");
        
        new_header->next = NULL;
        new_header->name = name;
        new_header->value = (char *) malloc(strlen(value) + 1);
        if (new_header->value == NULL)
            handle_error("malloc");
        strcpy(new_header->value, value);
        
        if (tail == NULL)
            http_resp->headers = new_header;            
        else
            tail->next = new_header;
    }
}

const char *get_response_header_value(struct Http_response_t *http_resp, const char *name)
{
    struct Header_field_t *node = http_resp->headers;

    while (node != NULL) {
        if (strcmp(node->name, name) == 0)
            break;
        node = node->next;
    }

    if (node == NULL)
        return NULL;

    return node->value;
}

void free_response(struct Http_response_t *http_resp)
{
    struct Header_field_t *header_current = http_resp->headers;
    struct Header_field_t *header_next;

    while (header_current != NULL) {
        header_next = header_current->next;
        free(header_current->value);
        free(header_current);
        header_current = header_next;
    }
}
