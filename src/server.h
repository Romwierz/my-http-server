#ifndef SERVER_H
#define SERVER_H

#include <stdbool.h>
#include <netinet/in.h>

extern bool kill_server;

void do_server_things(void);

#endif