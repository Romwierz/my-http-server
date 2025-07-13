#ifndef RESPONSE_H
#define RESPONSE_H

extern struct Http_response_t {
    int status_code;
    char *resp_fields;
    char msg_body[1024];
} http_resp;

void send_http_response(int status_code, const char *header_fields, const char *msg_body, int sockfd);

#endif
