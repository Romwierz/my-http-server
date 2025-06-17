#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <errno.h>

#define MY_PORT 8080
#define LISTEN_BACKLOG 5

#define handle_error(msg) do { perror(msg); exit(EXIT_FAILURE); } while (0)

const char * prompt = "> ";
const char * helpMsg = "h - help\ni - request\nq - disconnect\nk - kill server\n";
const char * msg = "HTTP 1.0 \"200 OK\"\n";
char buf[1];
int bytesRecv;

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
    int kill_server = 0;
    int disconnect_client = 0;

    int my_sockfd, client_sockfd;
    struct sockaddr_in my_addr, client_addr;
    socklen_t addr_size;

    my_sock_init(&my_sockfd, &my_addr);

    addr_size = sizeof(client_addr);

    while (1)
    {
        disconnect_client = 0;

        if ((client_sockfd = accept(my_sockfd, (struct sockaddr *)&client_addr, &addr_size)) == -1)
            handle_error("accept");
        printf("Found connection: client %d\n", client_sockfd);

        send_msg(prompt, client_sockfd);

        while ((bytesRecv = recv(client_sockfd, buf, 1, 0)) != -1)
        {
            switch (buf[0])
            {
            case 'h':
                /* send help */
                send_msg(helpMsg, client_sockfd);
                send_msg(prompt, client_sockfd);
                printf("Help message sent to client %d!\n", client_sockfd);
                break;
            case 'i':
                /* send msg */
                send_msg(msg, client_sockfd);
                send_msg(prompt, client_sockfd);
                printf("Message sent to client %d\n", client_sockfd);
                break;
            case 'q':
                disconnect_client = 1;
                break;
            case 'k':
                kill_server = 1;
                printf("Server killed...\n");
                break;
            case '\n':
                /* skip whitespaces */
                break;
            default:
                send_msg(helpMsg, client_sockfd);
                send_msg(prompt, client_sockfd);
                printf("Help message sent to client %d!\n", client_sockfd);
                break;
            }

            if (disconnect_client == 1) {
                if (close(client_sockfd) != -1)
                    printf("Disconnected with client %d\n\n", client_sockfd);
                break;
            }
            
            if (kill_server == 1)
                break;
        }

        if (kill_server == 1)
            break;
    }

    close(my_sockfd);
}

void send_msg(const char * ch, int client_sockfd)
{
    if (send(client_sockfd, ch, strlen(ch), 0) == -1)
        handle_error("send");
}
