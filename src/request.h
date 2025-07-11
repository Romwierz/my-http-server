#ifndef REQUEST_H
#define REQUEST_H

extern long bytes_in_file;

void handle_request(char *request, int sockfd);

#endif