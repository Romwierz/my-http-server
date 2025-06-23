#include <netinet/in.h>
#include <stdbool.h>
#include <errno.h>

#include "socket_comm.h"
#include "utils.h"
#include "messages.h"

int bytes_recv;

void socket_transmit(int sockfd, const char * msg)
{
    if (send(sockfd, msg, strlen(msg), 0) == -1)
        handle_error("send");
    
    /* send prompt only after sending the message */
    if (send(sockfd, PROMPT, strlen(PROMPT), 0) == -1)
        handle_error("send");
}

/** 
    if there are excessive bytes,
    discard them and write legal bytes into final_msg
**/
int socket_receive(int sockfd, char * msg, size_t len)
{
    bool excess_bytes_discarded = false;
    bool timeout = false;

    char final_msg[len];
    memset(final_msg, '\0', len);

    memset(msg, '\0', len);
    if ((bytes_recv = recv(sockfd, msg, len, 0)) == -1) {
        handle_error("recv");
    }

    memcpy(final_msg, msg, len);
    remove_trailing_newline(final_msg);
    printf("Message received: %s\n", final_msg);

    // return if msg ends with newline or if there is a null terminator in msg
    if (msg[len - 1] == '\n')
        return bytes_recv;
    for (size_t i = 0; i < len; i++)
    {
        if (msg[i] == '\0')
            return bytes_recv;
    }

    socket_set_timeout(sockfd, 0, 100);
    while (!excess_bytes_discarded)
    {
        int excess_bytes_recv;

        if ((excess_bytes_recv = recv(sockfd, msg, len, 0)) == -1) {
            // errno == 11 means that timeout has been reached
            if (errno == 11)
                excess_bytes_discarded = true;
            else
                handle_error("recv");
        }

        // do not exit while loop until there is null terminator in msg
        // but check only received bytes
        for (int i = 0; i < excess_bytes_recv; i++)
        {
            if (msg[i] == '\0')
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
