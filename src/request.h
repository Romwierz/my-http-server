#ifndef REQUEST_H
#define REQUEST_H

#define REQ_LINE_SIZE_MAX   150
#define METHOD_SIZE_MAX     25
#define URI_SIZE_MAX        100
#define VERSION_SIZE_MAX    25
#define REQ_FIELD_SIZE_MAX  50
#define REQ_FIELDS_MAX      5
#define MSG_BODY_SIZE_MAX   600

void handle_request(const char *request_raw, int sockfd);

#endif