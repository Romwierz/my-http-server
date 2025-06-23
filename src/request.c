#include <stdio.h>
#include <stdbool.h>

#include "request.h"
#include "server.h"
#include "socket_comm.h"
#include "messages.h"

void handle_request(char *request, int sockfd)
{
    switch (request[0])
    {
    case 'h':
        socket_transmit(sockfd, HELP_MSG);
        printf("Help message sent to client fd%d!\n\n", sockfd);
        break;
    case 'i':
        socket_transmit(sockfd, MSG);
        printf("Message sent to client fd%d\n\n", sockfd);
        break;
    case 'q':
        disconnect_client = true;
        printf("Disconnected with client fd%d\n\n", sockfd);
        break;
    case 'k':
        kill_server = true;
        printf("\nServer killed...\n");
        break;
    default:
        socket_transmit(sockfd, "");
        printf("Empty message sent to client fd%d\n\n", sockfd);
        break;
    }
}