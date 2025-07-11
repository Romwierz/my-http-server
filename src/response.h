#ifndef RESPONSE_H
#define RESPONSE_H

<<<<<<< HEAD
<<<<<<< HEAD
void http_response(int status_code, const char *header_fields, const char *msg_body, int sockfd);
=======
=======
>>>>>>> 7f6f39f (response: create Http_response_t struct to store all http response elements)
extern struct Http_response_t {
    int status_code;
    char *resp_fields;
    char msg_body[1024];
} http_resp;

<<<<<<< HEAD
void send_http_response(int status_code, const char *header_fields, const char *msg_body, int sockfd);
>>>>>>> b9b39c7 (refactor: rename response function)
=======
>>>>>>> 7f6f39f (response: create Http_response_t struct to store all http response elements)

#endif
