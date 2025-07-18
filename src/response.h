#ifndef RESPONSE_H
#define RESPONSE_H

#include "request.h"
#include "utils.h"

#define SEND_BUF_SIZE_MAX 4096

// https://datatracker.ietf.org/doc/html/rfc1945#section-9
// https://en.wikipedia.org/wiki/List_of_HTTP_status_codes
#define HTTP_STATUS_200 "HTTP/1.0 200 OK\r\n"
#define HTTP_STATUS_400 "HTTP/1.0 400 Bad Request\r\n"
#define HTTP_STATUS_403 "HTTP/1.0 403 Forbidden\r\n"
#define HTTP_STATUS_404 "HTTP/1.0 404 Not Found\r\n"
#define HTTP_STATUS_413 "HTTP/1.0 413 Payload Too Large\r\n"
#define HTTP_STATUS_414 "HTTP/1.0 414 URI Too Long\r\n"
#define HTTP_STATUS_501 "HTTP/1.0 501 Not Implemented\r\n"

struct Header_field_t {
    struct Header_field_t *next;
    const char *name;
    char *value;
};

struct Http_response_t {
    int status_code;
    struct Header_field_t *headers;
    char filepath[sizeof(SERVER_ROOT) + URI_SIZE_MAX];
    size_t filesize;
};

void send_http_response(struct Http_response_t *http_resp, int sockfd);
void add_response_header(struct Http_response_t *http_resp, const char *name, const char *value);
const char *get_response_header_value(struct Http_response_t *http_resp, const char *name);
void free_response(struct Http_response_t *http_resp);

#endif
