#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <errno.h>

#define MY_PORT 8080

int main(int argc, char *argv[])
{
    int my_sockfd, client_sockfd;
    struct sockaddr_in my_addr, client_addr;
    socklen_t addr_size;

    if ((my_sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1){
        fprintf(stderr, "Socket creation failed: %d\n", errno);
        exit(EXIT_FAILURE);
    }
    printf("Socket created: %d\n", my_sockfd);

    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(MY_PORT); // short, network byte order
    my_addr.sin_addr.s_addr = INADDR_ANY;
    memset(my_addr.sin_zero, '\0', sizeof my_addr.sin_zero);

    if (bind(my_sockfd, (struct sockaddr *)&my_addr, sizeof my_addr) == -1){
        fprintf(stderr, "Socket bind failed: %d\n", errno);
        exit(EXIT_FAILURE);
    }

    listen(my_sockfd, 1);
    printf("Server listening...\n\n");

    addr_size = sizeof(client_addr);
    client_sockfd = accept(my_sockfd, (struct sockaddr *) &client_addr, &addr_size);
    printf("Found connection!\n");

    close(my_sockfd);

    exit(EXIT_SUCCESS);
}