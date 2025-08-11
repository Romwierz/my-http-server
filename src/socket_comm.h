#ifndef SOCKET_COMM_H
#define SOCKET_COMM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if defined(_WIN32)
#else
    #include <unistd.h>
#endif
#define RECV_BUF_MAX 1024

size_t socket_transmit(int sockfd, const char *data, size_t data_size);
int socket_receive(int sockfd, char *recv_buf, size_t buf_size);

#endif
