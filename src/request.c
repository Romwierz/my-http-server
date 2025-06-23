#include <stdio.h>
#include <stdbool.h>

#include "request.h"
#include "server.h"
#include "socket_comm.h"
#include "messages.h"

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

static void response(enum Request_type req_type, enum Cmd_request_type cmd_req_type, int sockfd)
{
    switch (cmd_req_type)
    {
    case HELP:
        socket_transmit(sockfd, HELP_MSG);
        printf("Help message sent to client fd%d!\n\n", sockfd);
        break;
    case MESSAGE:
        socket_transmit(sockfd, MSG);
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

void handle_request(char *request, int sockfd)
{
    // Request_type req_type = get_req_type();
    enum Request_type req_type = CMD;
    enum Cmd_request_type cmd_req_type;

    switch (req_type)
    {
    case CMD:
        cmd_req_type = parse_cmd_request(request);
        break;
    default:
        break;
    }

    response(req_type, cmd_req_type, sockfd);
}
