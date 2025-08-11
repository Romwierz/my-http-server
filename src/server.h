#ifndef SERVER_H
#define SERVER_H

#include <stdbool.h>

#if defined(_WIN32)
    #include <winsock2.h>
    #define close closesocket
#else
    #include <netinet/in.h>
#endif

extern bool kill_server;

void do_server_things(void);

#endif