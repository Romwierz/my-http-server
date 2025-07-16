#ifndef RESPONSE_H
#define RESPONSE_H

    struct Header_field_t {
        struct Header_field_t *next;
        const char *name;
        char *value;
};

struct Http_response_t {
    int status_code;
    struct Header_field_t *headers;
    char msg_body[1024];
    long msg_body_size;
};

void send_http_response(struct Http_response_t *http_resp, int sockfd);

#endif
