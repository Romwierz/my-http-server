#ifndef RESPONSE_H
#define RESPONSE_H

#include "request.h"
#include "utils.h"

#define SEND_BUF_SIZE_MAX 4096

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
