#ifndef SERVER_H
#define SERVER_H

#include <netinet/in.h>

void my_sock_init(int *my_sockfd, struct sockaddr_in *my_addr);
void do_server_things(void);

#endif