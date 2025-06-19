/*
    This server can be tested with nc (netcat) utility.
    The simplest way to connect to a server running on a localhost and port 8080 is as follows:
    $ nc 127.0.0.1 8080
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <errno.h>
#include <stdbool.h>

#include "utils.h"
#include "messages.h"

#define MY_PORT 8080
#define LISTEN_BACKLOG 5

#define RECV_BUF_MAX 10

char recv_buf[RECV_BUF_MAX];
int bytesRecv;

void my_sock_init(int *my_sockfd, struct sockaddr_in *my_addr);
void do_server_things(void);
void send_msg(const char * msg, int client_sockfd);
int recv_msg(char * msg, size_t len, int client_sockfd);

int main(int argc, char *argv[])
{
    do_server_things();
    exit(EXIT_SUCCESS);
}

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
        send_msg(helloMsg, client_sockfd);
        
        while (recv_msg(recv_buf, RECV_BUF_MAX, client_sockfd) != -1)
        {            
            switch (recv_buf[0])
            {
            case 'h':
                send_msg(helpMsg, client_sockfd);
                printf("Help message sent to client fd%d!\n\n", client_sockfd);
                break;
            case 'i':
                send_msg(msg, client_sockfd);
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
                send_msg("", client_sockfd);
                printf("Empty message sent to client fd%d\n\n", client_sockfd);
                break;
            }

            if (disconnect_client) {
                if (close(client_sockfd) == -1)
                    handle_error("close client_sockfd");
                break;
            }
            
            if (kill_server)
                break;
        }

        if (kill_server)
            break;
    }

    if (close(my_sockfd) == -1)
        handle_error("close my_sockfd");
}

void send_msg(const char * msg, int client_sockfd)
{
    if (send(client_sockfd, msg, strlen(msg), 0) == -1)
        handle_error("send");
    
    /* send prompt only after sending the message */
    if (send(client_sockfd, prompt, strlen(prompt), 0) == -1)
        handle_error("send");

}

/** 
    if there are excessive bytes,
    discard them and write legal bytes into final_msg
**/
int recv_msg(char * msg, size_t len, int client_sockfd)
{
    bool excess_bytes_discarded = false;

    char final_msg[len];
    memset(final_msg, '\0', len);

    memset(msg, '\0', len);
    if ((bytesRecv = recv(client_sockfd, msg, len, 0)) == -1) {
        handle_error("recv");
        return bytesRecv;
    }

    memcpy(final_msg, msg, len);
    remove_trailing_newline(final_msg);
    printf("Message received: %s\n", final_msg);

    // return if msg ends with newline or if there is a null terminator in msg
    if (msg[len - 1] == '\n')
        return bytesRecv;
    for (size_t i = 0; i < len; i++)
    {
        if (msg[i] == '\0')
            return bytesRecv;
    }

    while (!excess_bytes_discarded)
    {
        memset(msg, '\0', len);
        if ((bytesRecv = recv(client_sockfd, msg, len, 0)) == -1) {
            handle_error("recv");
            return bytesRecv;
        }

        // do not exit while loop until there is null terminator in msg
        for (size_t i = 0; i < len - 1; i++)
        {
            if (msg[i] == '\0')
                excess_bytes_discarded = true;
        }
    }
    
    return bytesRecv;
}