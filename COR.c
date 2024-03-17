#include "COR.h"

int fd, errcode, udp_fd;
ssize_t n;
socklen_t addrlen;
struct addrinfo hints, *res;
struct sockaddr_in addr;
char buffer[1028];
char udp_buffer[1028];
char nodes[10];

char *TCP_Client(char *server_ip, char *server_port, char *msg)
{
    struct addrinfo hints, *res;
    int fd, n;
    ssize_t nbytes, nleft, nwritten, nread;
    char *ptr, buffer[128 + 1];

    fd = socket(AF_INET, SOCK_STREAM, 0); // TCP socket
    if (fd == -1)
        exit(1); // error

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;       // IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP socket

    n = getaddrinfo("127.0.0.1", "58001", &hints, &res); // IP e TCP do successor
    if (n != 0)                                          /*error*/
        exit(1);

    n = connect(fd, res->ai_addr, res->ai_addrlen);
    // perror("");
    if (n == -1) /*error*/
        exit(1);

    ptr = strcpy(buffer, msg);
    nbytes = strlen(msg);
    nleft = nbytes;
    while (nleft > 0)
    {
        nwritten = write(fd, ptr, nleft);
        if (nwritten <= 0) /*error*/
            exit(1);
        nleft -= nwritten;
        ptr += nwritten;
    }
    nleft = nbytes;
    ptr = buffer;
    while (nleft > 0)
    {
        nread = read(fd, ptr, nleft);
        if (nread == -1) /*error*/
            exit(1);
        else if (nread == 0)
            break; // closed by peer
        nleft -= nread;
        ptr += nread;
    }
    nread = nbytes - nleft;

    buffer[nread] = '\0';
    printf("echo: %s\n", buffer);
    close(fd);
    exit(0);
}

void UDP_client(char *msg)
{
    int fd, errcode;
    ssize_t n;
    socklen_t addrlen;
    struct addrinfo hints, *res;
    struct sockaddr_in addr;

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd == -1)
        exit(1);

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    errcode = getaddrinfo("tejo.tecnico.ulisboa.pt", "59000", &hints, &res);
    if (errcode != 0)
        exit(1);

    n = sendto(fd, msg, strlen(msg), 0, res->ai_addr, res->ai_addrlen);
    if (n == -1)
        exit(1);

    addrlen = sizeof(addr);
    n = recvfrom(fd, buffer, 200, 0, (struct sockaddr *)&addr, &addrlen);
    if (n == -1)
        exit(1);

    write(1, "echo: ", 6);
    write(1, buffer, n);

    freeaddrinfo(res);
    close(fd);

    return;
}

// Function to check if the given ID exists in the node list
int id_exists(char *node_list, char *id)
{
    char *token = strtok(node_list, " \n"); // Tokenize based on both space and newline

    while (token != NULL)
    {

        if (strcmp(token, id) == 0)
        {
            return 1; // ID exists
        }

        // If the lengths are different, the IDs are definitely different
        if (strlen(token) == strlen(id) && strncmp(token, id, strlen(id)) == 0)
        {
            return 1; // ID exists
        }

        token = strtok(NULL, " \n"); // Tokenize based on both space and newline
    }
    return 0; // ID doesn't exist
}

int join(char *ring, char *id, char *ip, char *tcp, char *succID, char *succIP, char *succTCP)
{

    // Formulate the message with "NODES r" format
    snprintf(nodes, sizeof(nodes), "NODES %s", ring);

    // Call UDP_client to send the message and receive node_list
    // char *node_list = UDP_client(nodes);
    
    // Send the nodes message do the UDP Client
    UDP_client(nodes);

    char *node_list = buffer;

    // fprintf(stderr, "LISTA: %s\n", node_list);

    // Process the node_list
    //fprintf(stderr,"Received response:\n%s\n", node_list);

    // Extract the succesor id, ip and tcp
    if (node_list)
    {
    if (sscanf(node_list, "NODELIST %s %s %s %s", ring, succID, succIP, succTCP) == 1)
    {
        char tcp_message[128];
        snprintf(tcp_message, sizeof(tcp_message), "ENTRY %s %s %s", id, ip, tcp);

        TCP_Client(id, tcp, tcp_message);

        if (sscanf(node_list, "NODELIST %s %s %s %s", ring, succID, succIP, succTCP) == 4)
        {
            fprintf(stderr, "SUCCESSOR ID: %s\nSUCCESSOR IP: %s\nSUCCESSOR TCP PORT: %s\n", succID, succIP, succTCP);

            // Check if the joining ID already exists in the ring
            if (id_exists(node_list, id))
            {
                fprintf(stderr, "ID %s already exists in the ring. Generating a new ID...\n", id);

                // Generate a new ID
                srand(time(NULL));
                int new_id = (rand() % 99) + 1; // Generate a random number between 1 and 99

                snprintf(id, 3, "%02d", new_id); // Ensure id has enough space

                fprintf(stderr, "GENERATED ID: %s\n", id);

                // Now, we need to recheck if the new ID exists in the node list
                // If it does, repeat the process until we find a unique ID
                while (id_exists(node_list, id))
                {
                    fprintf(stderr, "New ID %s already exists in the ring. Generating another new ID...\n", id);

                    new_id = (rand() % 99) + 1;
                    snprintf(id, 3, "%02d", new_id);
                }

                fprintf(stderr, "Final ID: %s\n", id);
            }

            fprintf(stderr, "Your new ID is: %s\n", id);

            // Establish a TCP connection to the successor node
            char tcp_message[128];
            snprintf(tcp_message, sizeof(tcp_message), "ENTRY %s %s %s", id, ip, tcp);
            TCP_Client(succIP, succTCP, tcp_message);
        }

        else
        {
            fprintf(stderr, "Failed to extract ID, IP, and TCP from the node list.\n");
        }
    }

    return 0;
}

int main(int argc, char *argv[]) // Recebe os argumentos do terminal, argc - contador, agrv - cria uma lista tamanho agrc com os
{
    // Verificar o numero de argumentos
    if (argc < 3 || argc > 5)
        return 1;

    char *regIP;
    char *regUDP;

    // Verificar se o utilizador fornece regIP e regUDP
    if (argv[3] == NULL)
    {
        regIP = strdup("193.136.138.142"); // Copia e aloca memória
        regUDP = strdup("59000");
    }
    else
    {
        regIP = argv[3];
        regUDP = argv[4];
    }
    // if (argv[4] == NULL)
    //     regUDP = "59000";
    // else
    //     regUDP = argv[4];

    // Debug messages
    printf("IP: %s\n", argv[1]);
    printf("TCP: %s\n", argv[2]);
    printf("regIP: %s\n", regIP);
    printf("regUDP: %s\n", regUDP);

    char msg[128], ring[4], id[3], ip[13], tcp[6], command[3], succID[3], succIP[13], succTCP[6];

    // Mensagem para o utilizador
    printf("Write the wanted command using the correct format:\n");

    scanf("%s %s %s", command, ring, id);

    // Verificaçao do que o utlizador escreve no terminal
    if (strcmp(command, "j") == 0)
    {
        if (strlen(ring) != 3)
        {
            printf("Anel inválido");
        }
        else if (strlen(id) != 2)
        {
            printf("No invalido");
        }
        else
        {
            // Hardcode do ip e tcp do no a registar
            char *ip = "127.0.0.1";
            char *tcp = "61000";

            join(ring, id, ip, tcp, succID, succIP, succTCP);
        }
    }
    free(regIP);
    free(regUDP);

    return 0;
}
