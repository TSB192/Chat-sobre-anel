#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

int main(void)
{
    struct addrinfo hints, *res;
    int fd, newfd, errcode;
    ssize_t n, nw;
    struct sockaddr addr;
    socklen_t addrlen;
    char *ptr, buffer[128];

    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        exit(1); // error

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;       // IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP socket
    hints.ai_flags = AI_PASSIVE;
    if ((errcode = getaddrinfo(NULL, "58001", &hints, &res)) != 0) /*error*/
        exit(1);

    if (bind(fd, res->ai_addr, res->ai_addrlen) == -1) /*error*/
        exit(1);
    if (listen(fd, 5) == -1) /*error*/
        exit(1);

    while (1)
    {
        addrlen = sizeof(addr);
        if ((newfd = accept(fd, &addr, &addrlen)) == -1)
            /*error*/ exit(1);

        // Read the message from the client
        n = read(newfd, buffer, 128);
        if (n == -1) /*error*/
            exit(1);

        // Check if the received message is "ENTRY"

        fprintf(stderr, "%s\n", buffer);

        if (strncmp(buffer, "ENTRY", 5) == 0)
        {
            // Construct and send the "SUCC" message
            char succ_message[128];
            snprintf(succ_message, sizeof(succ_message), "SUCC 61 127.0.0.1 50225");
            nw = write(newfd, succ_message, strlen(succ_message));
            if (nw <= 0) /*error*/
                exit(1);
        }
        close(newfd);
    }
    freeaddrinfo(res);
    close(fd);
    exit(0);
}
