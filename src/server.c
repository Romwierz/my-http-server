
#if defined(_WIN32)
    #include <winsock2.h>
#else
    #include <unistd.h>
    #include <netinet/in.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "server.h"
#include "utils.h"
#include "messages.h"
#include "socket_comm.h"
#include "request.h"

#define MY_PORT 8080U
#define LISTEN_BACKLOG 5

bool kill_server = false;

static int my_sock_init(struct sockaddr_in *my_addr)
{
    int my_sockfd;
    #if defined(_WIN32)
        WSADATA windows_socket_details;
        if (WSAStartup( MAKEWORD(2,2), &windows_socket_details) !=0) 
        {
            fprintf(stderr, "Winsock initialization failed: %d", WSAGetLastError());
            WSACleanup();
            exit(EXIT_FAILURE);
        }
        else
        {
            printf("Winsock version %d.%d initialized\n", 
            LOBYTE(windows_socket_details.wVersion), HIBYTE(windows_socket_details.wVersion));
        }
    #endif
    if ((my_sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1)
        handle_error("socket");
    printf("Socket created: fd%d\n\n", my_sockfd);

    int yes = 1;
    setsockopt(my_sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

    my_addr->sin_family = AF_INET;
    my_addr->sin_port = htons(MY_PORT);
    #if defined(_WIN32)
        my_addr->sin_addr.S_un.S_addr = INADDR_ANY;
    #else
        my_addr->sin_addr.s_addr = INADDR_ANY;
    #endif
    memset(my_addr->sin_zero, '\0', sizeof my_addr->sin_zero);

    if (bind(my_sockfd, (struct sockaddr *)my_addr, sizeof(*my_addr)) == -1)
        handle_error("bind");

    if (listen(my_sockfd, LISTEN_BACKLOG) == -1)
        handle_error("listen");

    return my_sockfd;
}

void do_server_things(void)
{
    // extra character for null terminator
    char recv_buf[RECV_BUF_MAX + 1] = { 0 };

    int my_sockfd, client_sockfd;
    struct sockaddr_in my_addr, client_addr;
    #if defined(_WIN32)
        int addr_size;
    #else
        socklen_t addr_size;
    #endif

    my_sockfd = my_sock_init(&my_addr);

    addr_size = sizeof(client_addr);

    while (1)
    {
        printf(ANSI_COLOR_CYAN "Server listening..." ANSI_COLOR_RESET "\n");

        if ((client_sockfd = accept(my_sockfd, (struct sockaddr *)&client_addr, &addr_size)) == -1)
            handle_error("accept");
        printf("Found connection: client fd%d\n\n", client_sockfd);
        
        if (socket_receive(client_sockfd, recv_buf, RECV_BUF_MAX) != -1)
            handle_request(recv_buf, client_sockfd);

        if (close(client_sockfd) == -1)
            handle_error("close client_sockfd");

        if (kill_server)
            break;
    }

    if (close(my_sockfd) == -1)
        handle_error("close my_sockfd");
}
