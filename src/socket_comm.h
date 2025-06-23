#ifndef SOCKET_COMM_H
#define SOCKET_COMM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define RECV_BUF_MAX 10

extern char recv_buf[RECV_BUF_MAX + 1];

void socket_transmit(int sockfd, const char * msg);
int socket_receive(int sockfd, char * msg, size_t len);

void socket_set_timeout(int sockfd, int sec, int usec);
void socket_disable_timeout(int sockfd);

#endif
