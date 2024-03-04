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
        // n = sendto(fd, buffer, nread, 0, &addr, addrlen);
        // if (n == -1) /*error*/
        // exit(1);

        buffer[nread] = '\0';

        // Check if the received message is "NODES"
        int num_letras = 4;

        if (strncmp(buffer, "NODES", num_letras) == 0)
        {
            // Extract the ring ID from the message
            char ringID[4];
            memcpy(ringID, buffer + 6, 3); // Copy 3 characters starting from position 6
            ringID[3] = '\0';              // Null-terminate the string

            printf("Received message with ring ID: %s\n", ringID);

            // Replace this with code to generate the node list for the specified ring ID
            char nodeList[MAX_NODES][128] = {"01 193.136.138.142 5900", "61 127.0.0.1 50225"};
            int numNodes = 3; // Number of nodes in the list

            // Construct the response message with "NODELIST" followed by ringID
            char response[128 * MAX_NODES];
            sprintf(response, "NODELIST %s\n", ringID);

            // Concatenate the node list into the response message
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