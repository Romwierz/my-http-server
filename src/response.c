#include <stdio.h>

#include "response.h"
#include "request.h"
#include "socket_comm.h"
#include "messages.h"

void send_http_response(struct Http_response_t *http_resp, int sockfd)
{
    switch (http_resp->status_code)
    {
    case 200:
        socket_transmit(sockfd, HTTP_STATUS_200, strlen(HTTP_STATUS_200));
        socket_transmit(sockfd, http_resp->header_field.name, strlen(http_resp->header_field.name));
        socket_transmit(sockfd, CRLF, strlen(CRLF));
        socket_transmit(sockfd, CRLF, strlen(CRLF));
        socket_transmit(sockfd, http_resp->msg_body, http_resp->msg_body_size);
        break;
    case 400:
        socket_transmit(sockfd, HTTP_STATUS_400, strlen(HTTP_STATUS_400));
        break;
    case 403:
        socket_transmit(sockfd, HTTP_STATUS_403, strlen(HTTP_STATUS_403));
        break;
    case 404:
        socket_transmit(sockfd, HTTP_STATUS_404, strlen(HTTP_STATUS_404));
        break;
    case 413:
        socket_transmit(sockfd, HTTP_STATUS_413, strlen(HTTP_STATUS_413));
        break;
    case 414:
        socket_transmit(sockfd, HTTP_STATUS_414, strlen(HTTP_STATUS_414));
        break;
    case 501:
        socket_transmit(sockfd, HTTP_STATUS_501, strlen(HTTP_STATUS_501));
        break;
    default:
        break;
    }

    printf("HTTP response %d sent to client fd%d\n\n", http_resp->status_code, sockfd);
}