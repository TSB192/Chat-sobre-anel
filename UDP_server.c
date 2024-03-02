#include "COR.h"

int fd, errcode;
ssize_t n;
socklen_t addrlen;
struct addrinfo hints, *res;
struct sockaddr_in addr;
char buffer[128];

int main(void)
{
    struct addrinfo hints, *res;
    int fd, errcode;
    struct sockaddr addr;
    socklen_t addrlen;
    ssize_t n, nread;
    char buffer[128];

    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
        exit(1); // error

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;      // IPv4
    hints.ai_socktype = SOCK_DGRAM; // UDP socket
    hints.ai_flags = AI_PASSIVE;
    if ((errcode = getaddrinfo(NULL, PORT, &hints, &res)) != 0) /*error*/
        exit(1);

    if (bind(fd, res->ai_addr, res->ai_addrlen) == -1) /*error*/
        exit(1);

    while (1)
    {
        addrlen = sizeof(addr);
        nread = recvfrom(fd, buffer, sizeof(buffer), 0, &addr, &addrlen);
        if (nread == -1) /*error*/
            exit(1);
        //n = sendto(fd, buffer, nread, 0, &addr, addrlen);
        //if (n == -1) /*error*/
        //    exit(1);
        
        buffer[nread] = '\0';

        // Check if the received message is "NODES"
        if (strcmp(buffer, "NODES") == 0)
        {
            // Replace this with code to generate the node list
            char nodeList[MAX_NODES][128] = {"NODE LIST", "Node 1", "Node 2", "Node 3"};
            int numNodes = 3; // Number of nodes in the list

            // Concatenate the node list into a single string
            char response[128 * MAX_NODES];
            strcpy(response, "");
            for (int i = 0; i < numNodes; ++i)
            {
                strcat(response, nodeList[i]);
                strcat(response, "\n"); // Add newline between nodes
            }

            // Send the node list back to the client
            n = sendto(fd, response, strlen(response), 0, &addr, addrlen);
            if (n == -1) /*error*/
                exit(1);
        }
    }

    freeaddrinfo(res);
    close(fd);
    exit(0);
}