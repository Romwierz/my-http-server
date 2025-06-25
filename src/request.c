#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "request.h"
#include "server.h"
#include "socket_comm.h"
#include "messages.h"
#include "utils.h"

static void http_response(int status_code, int sockfd)
{
    switch (status_code)
    {
    case 200:
        socket_transmit(sockfd, HTTP_STATUS_200);
        break;
    case 400:
        socket_transmit(sockfd, HTTP_STATUS_400);
        break;
    case 404:
        socket_transmit(sockfd, HTTP_STATUS_404);
        break;
    default:
        break;
    }

    printf("HTTP response %d sent to client fd%d\n\n", status_code, sockfd);
}

void handle_request(char *request, int sockfd)
{
    int status_code = parse_http_request(request);

    http_response(status_code, sockfd);
}
