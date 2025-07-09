#ifndef SERVER_H
#define SERVER_H

#include <stdbool.h>
#include <netinet/in.h>

extern bool kill_server;

int my_sock_init(struct sockaddr_in *my_addr);
void do_server_things(void);

#endif