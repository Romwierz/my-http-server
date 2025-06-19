#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <errno.h>
#include <stdbool.h>

#include "server.h"
#include "utils.h"
#include "messages.h"
#include "socket_comm.h"

#define MY_PORT 8080U
#define LISTEN_BACKLOG 5
#define RECV_BUF_MAX 10

char recv_buf[RECV_BUF_MAX];

void my_sock_init(int *my_sockfd, struct sockaddr_in *my_addr)
{
    if ((*my_sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1)
        handle_error("socket");
    printf("Socket created: fd%d\n\n", *my_sockfd);

    int yes = 1;
    setsockopt(*my_sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

    my_addr->sin_family = AF_INET;
    my_addr->sin_port = htons(MY_PORT);
    my_addr->sin_addr.s_addr = INADDR_ANY;
    memset(my_addr->sin_zero, '\0', sizeof my_addr->sin_zero);

    if (bind(*my_sockfd, (struct sockaddr *)my_addr, sizeof(*my_addr)) == -1)
        handle_error("bind");

    if (listen(*my_sockfd, LISTEN_BACKLOG) == -1)
        handle_error("listen");
    printf("Server listening...\n\n");
}

void do_server_things(void)
{
    bool kill_server = false;
    bool disconnect_client = false;

    int my_sockfd, client_sockfd;
    struct sockaddr_in my_addr, client_addr;
    socklen_t addr_size;

    my_sock_init(&my_sockfd, &my_addr);

    addr_size = sizeof(client_addr);

    while (1)
    {
        disconnect_client = false;

        if ((client_sockfd = accept(my_sockfd, (struct sockaddr *)&client_addr, &addr_size)) == -1)
            handle_error("accept");
        printf("Found connection: client fd%d\n\n", client_sockfd);
        socket_transmit(client_sockfd, HELLO_MSG);
        
        while (socket_receive(client_sockfd, recv_buf, RECV_BUF_MAX) != -1)
        {            
            switch (recv_buf[0])
            {
            case 'h':
                socket_transmit(client_sockfd, HELP_MSG);
                printf("Help message sent to client fd%d!\n\n", client_sockfd);
                break;
            case 'i':
                socket_transmit(client_sockfd, MSG);
                printf("Message sent to client fd%d\n\n", client_sockfd);
                break;
            case 'q':
                disconnect_client = true;
                printf("Disconnected with client fd%d\n\n", client_sockfd);
                break;
            case 'k':
                kill_server = true;
                printf("\nServer killed...\n");
                break;
            default:
                socket_transmit(client_sockfd, "");
                printf("Empty message sent to client fd%d\n\n", client_sockfd);
                break;
            }

            if (kill_server || disconnect_client) {
                if (close(client_sockfd) == -1)
                    handle_error("close client_sockfd");
                break;
            }
        }

        if (kill_server)
            break;
    }

    if (close(my_sockfd) == -1)
        handle_error("close my_sockfd");
}
