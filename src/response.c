#include <stdio.h>

#include "socket_comm.h"
#include "messages.h"

void http_response(int status_code, const char *header_fields, const char *msg_body, int sockfd)
{
    switch (status_code)
    {
    case 200:
        socket_transmit(sockfd, HTTP_STATUS_200);
        socket_transmit(sockfd, "Content-Type: text/html");
        socket_transmit(sockfd, "\r\n\r\n");
        socket_transmit(sockfd, msg_body);
        break;
    case 400:
        socket_transmit(sockfd, HTTP_STATUS_400);
        break;
    case 403:
        socket_transmit(sockfd, HTTP_STATUS_403);
        break;
    case 404:
        socket_transmit(sockfd, HTTP_STATUS_404);
        break;
    case 413:
        socket_transmit(sockfd, HTTP_STATUS_413);
        break;
    case 414:
        socket_transmit(sockfd, HTTP_STATUS_414);
        break;
    case 501:
        socket_transmit(sockfd, HTTP_STATUS_501);
        break;
    default:
        break;
    }

    printf("HTTP response %d sent to client fd%d\n\n", status_code, sockfd);
}