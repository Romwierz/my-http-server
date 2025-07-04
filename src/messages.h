#ifndef MESSAGES_H
#define MESSAGES_H

#define PROMPT      "> "
#define HELLO_MSG   "Hello, my name is server :-)\n"
#define HELP_MSG    "h - help\ni - request\nq - disconnect\nk - kill server\n"

// https://datatracker.ietf.org/doc/html/rfc1945#section-9
// https://en.wikipedia.org/wiki/List_of_HTTP_status_codes
#define HTTP_STATUS_200 "HTTP/1.0 200 OK\r\n"
#define HTTP_STATUS_400 "HTTP/1.0 400 Bad Request\r\n"
#define HTTP_STATUS_403 "HTTP/1.0 403 Forbidden\r\n"
#define HTTP_STATUS_404 "HTTP/1.0 404 Not Found\r\n"
#define HTTP_STATUS_413 "HTTP/1.0 413 Payload Too Large\r\n"
#define HTTP_STATUS_414 "HTTP/1.0 414 URI Too Long\r\n"
#define HTTP_STATUS_501 "HTTP/1.0 501 Not Implemented\r\n"

#endif