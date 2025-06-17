#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <errno.h>

#define MY_PORT 8080
#define LISTEN_BACKLOG 5

#define handle_error(msg) do { perror(msg); exit(EXIT_FAILURE); } while (0)

const char * msg = "HTTP 1.0 \"200 OK\"\n";

void my_sock_init(int *my_sockfd, struct sockaddr_in *my_addr);
void do_server_things(void);
void send_msg(const char * ch, int client_sockfd);

int main(int argc, char *argv[])
{
    do_server_things();
    exit(EXIT_SUCCESS);
}

void my_sock_init(int *my_sockfd, struct sockaddr_in *my_addr)
{
    if ((*my_sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1)
        handle_error("socket");
    printf("Socket created: %d\n\n", *my_sockfd);

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
    int my_sockfd, client_sockfd;
    struct sockaddr_in my_addr, client_addr;
    socklen_t addr_size;

    my_sock_init(&my_sockfd, &my_addr);

    addr_size = sizeof(client_addr);
    if ((client_sockfd = accept(my_sockfd, (struct sockaddr *) &client_addr, &addr_size)) == -1)
        handle_error("accept");
    printf("Found connection!\n");

    if (send(client_sockfd, msg, strlen(msg), 0) == -1)
        handle_error("send");

    close(my_sockfd);
}

void send_msg(const char * ch, int client_sockfd)
{
    if (send(client_sockfd, ch, strlen(ch), 0) == -1)
        handle_error("send");
}
