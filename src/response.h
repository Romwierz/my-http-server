#ifndef RESPONSE_H
#define RESPONSE_H

struct Http_response_t {
    int status_code;
    struct Header_field_t {
        struct Header_field_t *next;
        const char *name;
        char *value;
    } header_field;
    char msg_body[1024];
};

void send_http_response(int status_code, struct Http_response_t *http_resp, int sockfd);

#endif
