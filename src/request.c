#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "request.h"
#include "server.h"
#include "socket_comm.h"
#include "messages.h"
#include "utils.h"

enum Request_type {
	CMD, HTTP, INVALID
};

enum Cmd_request_type {
	HELP, MESSAGE, DISCONNECT, KILL_SERVER, EMPTY
};

static enum Cmd_request_type parse_cmd_request(char *request)
{
    switch (request[0])
    {
    case 'h':
        return HELP;
    case 'i':
        return MESSAGE;
    case 'q':
        return DISCONNECT;
    case 'k':
        return KILL_SERVER;
    default:
        return EMPTY;
    }
}

static int parse_http_request(char *request)
{
    char *new_location = remove_leading_whitespaces(request);
    if (new_location != NULL)
        request = new_location;
    
    if (strncmp("GET", request, sizeof("GET")) == 0)
        return 200;
    
    return 400;
}

static void cmd_response(enum Cmd_request_type cmd_req_type, int sockfd)
{
    switch (cmd_req_type)
    {
    case HELP:
        socket_transmit(sockfd, HELP_MSG);
        printf("Help message sent to client fd%d!\n\n", sockfd);
        break;
    case MESSAGE:
        socket_transmit(sockfd, HTTP_STATUS_200);
        printf("Message sent to client fd%d\n\n", sockfd);
        break;
    case DISCONNECT:
        disconnect_client = true;
        printf("Disconnected with client fd%d\n\n", sockfd);
        break;
    case KILL_SERVER:
        kill_server = true;
        printf("\nServer killed...\n");
        break;
    default:
        socket_transmit(sockfd, "");
        printf("Empty message sent to client fd%d\n\n", sockfd);
        break;
    }
}

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
