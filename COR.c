#include "COR.h"

int fd, errcode;
ssize_t n;
socklen_t addrlen;
struct addrinfo hints, *res;
struct sockaddr_in addr;
char buffer[128];
char nodes[10];

char *UDP_client(char *msg)
{
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd == -1)
        exit(1);

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    errcode = getaddrinfo("127.0.0.1", PORT, &hints, &res);
    if (errcode != 0)
        exit(1);

    n = sendto(fd, msg, sizeof msg, 0, res->ai_addr, res->ai_addrlen);
    if (n == -1)
        exit(1);

    addrlen = sizeof(addr);
    n = recvfrom(fd, buffer, sizeof(buffer), 0, (struct sockaddr *)&addr, &addrlen);
    if (n == -1)
        exit(1);

    buffer[n] = '\0';

    write(1, "echo: ", 6);
    write(1, msg, strlen(nodes));
    write(1, "\n", 1);

    freeaddrinfo(res);
    close(fd);

    //Close buffer

    return buffer;
}

int join(char *ring, char *id, char *ip, char *tcp, char *succ, char *succsucc, char *pred)
{
    // Formulate the message
    snprintf(nodes, sizeof(nodes), "NODES");

    // Call UDP_client to send the message and receive response
    char *response = UDP_client(nodes);
    
    // Process the response
    printf("Received response: %s\n", response);

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
        regIP = "193.136.138.142";
    else
        regIP = argv[3];

    if (argv[4] == NULL)
        regUDP = "59000";
    else
        regUDP = argv[4];

    // for (int i = 0; i < argc; i++)
    //{
    //     printf("%d-%s\n", i, argv[i]);
    // }

    char msg[128], ring[4], id[3], ip[13], tcp[6], command[3];

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
            join(ring, id, ip, tcp);
    }
    printf("adoro o sopas");
    return 0;
}
