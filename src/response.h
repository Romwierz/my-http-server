#ifndef RESPONSE_H
#define RESPONSE_H

void http_response(int status_code, const char *header_fields, const char *msg_body, int sockfd);

#endif
