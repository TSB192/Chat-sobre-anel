#include "COR.h"

int fd, errcode, udp_fd, newfd, counter;
int succ_fd = -1, pred_fd = -1, my_fd = -1;
fd_set rfds;
ssize_t n;
socklen_t addrlen;
struct addrinfo hints, *res;
struct sockaddr_in addr;
char buffer[1028];
char udp_buffer[1028];
char nodes[10];

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

    fprintf(stderr, "n: %ld\n", n);

    write(1, "echo: ", 6);
    write(1, buffer, n);

    freeaddrinfo(res);
    close(fd);

    return;
}

void TCP_Client(char *ip, char *port, char *msg)
{

    fprintf(stderr, "ENTREI NO TCP SERVER\n");

    int fd, errcode;
    ssize_t n;
    socklen_t addrlen;
    struct addrinfo hints, *res;
    struct sockaddr_in addr;
    char buffer[200];

    memset(buffer, 0, 200);

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1)
        exit(1);
    succ_fd = fd;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    errcode = getaddrinfo(ip, port, &hints, &res);

    if (errcode != 0)
        exit(1);

    n = connect(fd, res->ai_addr, res->ai_addrlen);

    if (n == -1)
        exit(1);

    n = write(fd, msg, strlen(msg));
    if (n == -1)
        exit(1);

    // n=read(fd,buffer,200);
    // if(n==-1) exit(1);

    // write(1,"echo: ",6); write(1,buffer,n);

    freeaddrinfo(res);
    // close(fd);

    return;
}

void TCP_Server(char *port)
{

    fprintf(stderr, "ENTREI NO TCP SERVER\n");

    int fd, errcode, newfd;
    ssize_t n;
    socklen_t addrlen;
    struct addrinfo hints, *res;
    struct sockaddr_in addr;
    char buffer[200];

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1)
        exit(1);

    my_fd = fd;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    errcode = getaddrinfo(NULL, port, &hints, &res);
    if (errcode != 0)
        exit(1);

    n = bind(fd, res->ai_addr, res->ai_addrlen);
    if (n == -1)
        exit(1);

    if (listen(fd, 5) == -1)
        exit(1);

    /*while(1){
        addrlen=sizeof(addr);
        if((newfd=accept(fd,(struct sockaddr*)&addr,&addrlen))==-1) exit(1);

        n=read(newfd,buffer,200);
        if(n==-1) exit(1);
        write(1,"received: ",10);write(1,buffer,n);

        n=write(newfd,buffer,n);
        if(n==-1) exit(1);

        close(newfd);
    }*/

    freeaddrinfo(res);
    // close(fd);

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

        token = strtok(NULL, " \n"); // Tokenize based on both space and newline
    }
    return 0; // ID doesn't exist
}

int __str_len(const char *str, int sep)
{
    int i;

    i = 0;
    while (str && str[i] && sep != str[i])
        ++i;
    return (i);
}

char **__ft_split(const char *str, int sep)
{
    char **strings;
    static int size;
    int i;
    char *word;

    i = 0;
    word = NULL;
    while (*str && sep == *str)
        str++;
    i = __str_len(str, sep);
    if (i)
        word = strndup(str, i);
    if (word && ++size)
        strings = __ft_split(str + i, sep);
    else
        strings = (char **)calloc((size + 1), sizeof(char *));
    strings[size--] = word;
    if (size < 0)
        ++size;
    return (strings);
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

    // Process the node_list
    // fprintf(stderr, "Received response:\n%s\n", node_list);

    // Extract the succesor id, ip and tcp

    char **split = __ft_split(__ft_split(node_list, '\n')[1], ' ');

    succID = split[0];
    succIP = split[1];
    succTCP = split[2];

    if (succID == NULL)
    {
        TCP_Server(tcp);

        // Establish a TCP connection to the successor node
        char tcp_message[128];
        snprintf(tcp_message, sizeof(tcp_message), "REG %s %s %s", id, ip, tcp);

        // If node is empty then the node is its own successor
        TCP_Client(id, tcp, tcp_message);
    }

    if (succID != NULL)
    {
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

        TCP_Server(tcp);

        // Establish a TCP connection to the successor node
        char tcp_message[128];
        snprintf(tcp_message, sizeof(tcp_message), "REG %s %s %s %s", ring, id, ip, tcp);

        fprintf(stderr, "LOCAL REG: %s\n", tcp_message);

        TCP_Client(succIP, succTCP, tcp_message);

        fprintf(stderr, "BAZEI DO TCP\n");
    }

    char registo[128];
    snprintf(registo, sizeof(registo), "REG %s %s %s %s", ring, id, ip, tcp);

    fprintf(stderr, "LOCAL REG: %s\n", registo);

    UDP_client(registo);

    return 0;
}

// void Show_topology(char *id, char *succID, char *succIP, char *succTCP, char *succsuccID, char *succsuccIP, char *succsuccTCP, char *predID)
//{
//     printf("%s\n", id);
//     printf("%s %s %s\n", succID, succIP, succTCP);
//     printf("%s %s %s\n", succsuccID, succsuccIP, succsuccTCP);
//     printf("%s\n", predID);
//     return;
// }

int process_commands()
{
    char msg[128], ring[4], id[3], ip[13], tcp[6], command[3], succID[3], succIP[13], succTCP[6], succsuccID[3], succsuccIP[16], succsuccTCP[6], predID[3];

    // Prompt the user for a command
    printf("Write the wanted command using the correct format:\n");
 
    scanf("%s %s %s", command, ring, id);

    fprintf(stderr, "COMANDO -> %s \n", command);
    fprintf(stderr, "RING -> %s \n", ring);
    fprintf(stderr, "ID -> %s \n", id);

    // If the command is 'x', exit the program immediately
    if (strcmp(command, "x") == 0)
    {

        fprintf(stderr, "ENTREI NO COMANDO X \n" );

        return 0; // Exit the program
    }

    // If the command is 'j', prompt the user for ring and id
    else if (strcmp(command, "j") == 0)
    {
        // Scan the ring and id
        if (scanf("%s %s", ring, id) != 2)
        {
            printf("Invalid input.\n");
            return 1; // Continue running the program
        }

        // Validate ring and id
        if (strlen(ring) != 3 || strlen(id) != 2)
        {
            printf("Invalid ring or id.\n");
            return 1; // Continue running the program
        }

        // Hardcode the ip and tcp of the node to register
        char *ip = "127.0.0.1";
        char *tcp = "60001";

        join(ring, id, ip, tcp, succID, succIP, succTCP);
    }

    return 1; // Continue running the program
}


int main(int argc, char *argv[]) // Recebe os argumentos do terminal, argc - contador, agrv - cria uma lista tamanho agrc com os
{

    char msg[128], ring[4], id[3], ip[13], tcp[6], command[3], succID[3], succIP[13], succTCP[6], succsuccID[3], succsuccIP[16], succsuccTCP[6], predID[3];

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
    // printf("IP: %s\n", argv[1]);
    // printf("TCP: %s\n", argv[2]);
    // printf("regIP: %s\n", regIP);
    // printf("regUDP: %s\n", regUDP);

    // Mensagem para o utilizador
    // printf("Write the wanted command using the correct format:\n");
    //
    // scanf("%s %s %s", command, ring, id);

    // Verificaçao do que o utlizador escreve no terminal
    // if (strcmp(command, "j") == 0)
    //{
    //    if (strlen(ring) != 3)
    //    {
    //        printf("Anel inválido");
    //    }
    //    else if (strlen(id) != 2)
    //    {
    //        printf("No invalido");
    //    }
    //    else
    //    {
    //        // Hardcode do ip e tcp do no a registar
    //        char *ip = "127.0.0.1";
    //        char *tcp = "60001";
    //
    //        join(ring, id, ip, tcp, succID, succIP, succTCP);
    //    }
    //}

    // Show topology when user types 'st'
    // if (strcmp(command, "st") == 0)
    //{
    //    Show_topology(id, succID, succIP, succTCP, succsuccID, succsuccIP, succsuccTCP, predID);
    //}

    while (1)
    {
        process_commands();
    }

    // while (1)
    //{
    //     fd = Biggest_fd();
    //     printf("\n");
    //     FD_ZERO(&rfds);         /* remover todos os descritores do conjunto */
    //     FD_SET(0, &rfds);       /* adicionar o descritor 0 (stdin) ao conjunto */
    //     FD_SET(my_fd, &rfds);   /* adicionar o descritor fd (socket TCP) ao conjunto */
    //     FD_SET(pred_fd, &rfds); /* adicionar o descritor 0 (stdin) ao conjunto */
    //     FD_SET(succ_fd, &rfds); /* adicionar o descritor fd (socket TCP) ao conjunto */
    //
    //    counter = select(fd + 1, &rfds, (fd_set *)NULL, (fd_set *)NULL, (struct timeval *)NULL);
    //    if (counter == -1)
    //        exit(1);
    //
    //    if (FD_ISSET(0, &rfds) != 0)
    //    {
    //        if (Read_input() != 0)
    //        {
    //            break;
    //        }
    //    }
    //    if (FD_ISSET(my_fd, &rfds) != 0)
    //    {
    //        addrlen = sizeof(addr);
    //        if ((newfd = accept(my_fd, (struct sockaddr *)&addr, &addrlen)) == -1)
    //            exit(1);
    //        Read_buffer_tcp(newfd);
    //    }
    //    if (FD_ISSET(pred_fd, &rfds) != 0)
    //    {
    //        Read_buffer_tcp(pred_fd);
    //    }
    //    if (FD_ISSET(succ_fd, &rfds) != 0)
    //    {
    //        Read_buffer_tcp(succ_fd);
    //    }
    //    // printf("new cycle");
    //}

    free(regIP);
    free(regUDP);

    return 0;
}
