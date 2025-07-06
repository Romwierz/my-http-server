#include <netinet/in.h>
#include <stdbool.h>
#include <errno.h>

#include "socket_comm.h"
#include "utils.h"
#include "messages.h"

void socket_transmit(int sockfd, const char * msg)
{
    if (send(sockfd, msg, strlen(msg), 0) == -1)
        handle_error("send");
}

/** 
    if there are excessive bytes,
    just read them into recv_trash_buf
**/
int socket_receive(int sockfd, char *recv_buf, size_t buf_size)
{
    char recv_trash_buf[RECV_BUF_MAX + 1];
    int bytes_recv;
    bool excess_bytes_discarded = false;

    memset(recv_buf, '\0', buf_size);
    if ((bytes_recv = recv(sockfd, recv_buf, buf_size, 0)) == -1) {
        handle_error("recv");
    }

    printf("Data received:\n%s", recv_buf);

    // return if there is a null terminator in recv_buf
    for (size_t i = 0; i < buf_size; i++)
    {
        if (recv_buf[i] == '\0')
            return bytes_recv;
    }

    socket_set_timeout(sockfd, 0, 100);
    while (!excess_bytes_discarded)
    {
        int excess_bytes_recv;

        if ((excess_bytes_recv = recv(sockfd, recv_trash_buf, buf_size, 0)) == -1) {
            // errno == 11 means that timeout has been reached
            if (errno == 11) {
                excess_bytes_discarded = true;
                break;
            }
            else
                handle_error("recv");            
        }

        // do not exit while loop until there is null terminator in recv_trash_buf
        // but check only received bytes
        for (int i = 0; i < excess_bytes_recv; i++)
        {
            if (recv_trash_buf[i] == '\0')
                excess_bytes_discarded = true;
        }
    }
    socket_disable_timeout(sockfd);
    
    return bytes_recv;
}

void socket_set_timeout(int sockfd, int sec, int usec)
{
    struct timeval tv;
    tv.tv_sec = sec;
    tv.tv_usec = usec;
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv, sizeof tv);
}

void socket_disable_timeout(int sockfd)
{
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv, sizeof tv);
}
