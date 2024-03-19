#include "COR.h"

int errcode, udp_fd, newfd, counter;
int fd;
int succ_fd;
int pred_fd;
int my_fd;
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
    {
        exit(1);
    }
    n = connect(fd, res->ai_addr, res->ai_addrlen);

    if (n == -1)
    {
        perror("connect");
        exit(1);
    }

    // if (n == -1)
    //     exit(1);

    n = write(fd, msg, strlen(msg));

    if (n == -1)
    {
        perror("write");
        exit(1);
    }
    // if (n == -1)
    //     exit(1);

    // n=read(fd,buffer,200);
    // if(n==-1) exit(1);

    // write(1,"echo: ",6); write(1,buffer,n);

    freeaddrinfo(res);
    close(fd);

    return;
}

void TCP_Server(char *port)
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

    if ((errcode = getaddrinfo(NULL, port, &hints, &res)) != 0) /*error*/
        exit(1);

    if (bind(fd, res->ai_addr, res->ai_addrlen) == -1) /*error*/
        exit(1);

    if (listen(fd, 5) == -1) /*error*/
        exit(1);

    fprintf(stderr, "Cheguei aqui")

    while (1)
    {
        addrlen = sizeof(addr);
        if ((newfd = accept(fd, &addr, &addrlen)) == -1)
            /*error*/ exit(1);
        while ((n = read(newfd, buffer, 128)) != 0)
        {
            if (n == -1) /*error*/
                exit(1);
            ptr = &buffer[0];
            while (n > 0)
            {
                if ((nw = write(newfd, ptr, n)) <= 0) /*error*/
                    exit(1);
                n -= nw;
                ptr += nw;
            }
        }
        close(newfd);
    }
    
    freeaddrinfo(res);
    close(fd);
    return;
}

//void TCP_Server(char *port)
//{
//    int fd, errcode, newfd;
//    ssize_t n;
//    socklen_t addrlen;
//    struct addrinfo hints, *res;
//    struct sockaddr_in addr;
//    char buffer[200];
//
//    fd = socket(AF_INET, SOCK_STREAM, 0);
//
//    if (fd == -1)
//        exit(1);
//
//    my_fd = fd;
//
//    memset(&hints, 0, sizeof hints);
//    hints.ai_family = AF_INET;
//    hints.ai_socktype = SOCK_STREAM;
//    hints.ai_flags = AI_PASSIVE;
//
//    errcode = getaddrinfo(NULL, port, &hints, &res);
//
//    fprintf(stderr, "ERRORCODE NO TCP SERVER -> %d\n", errcode);
//
//    if (errcode != 0)
//        exit(1);
//
//    n = bind(fd, res->ai_addr, res->ai_addrlen);
//
//    fprintf(stderr, "N NO TCP SERVER -> %ld\n", n);
//
//    if (n == -1)
//        exit(1);
//
//    if (listen(fd, 5) == -1)
//        exit(1);
//
    //while(1)
    //{
    //    addrlen=sizeof(addr);
    //    if((newfd=accept(fd,(struct sockaddr*)&addr,&addrlen))==-1) exit(1);
//
    //    n=read(newfd,buffer,200);
    //    if(n==-1) exit(1);
    //    write(1,"received: ",10);write(1,buffer,n);
//
    //    n=write(newfd,buffer,n);
    //    if(n==-1) exit(1);
//
    //    close(newfd);
    //}

//    freeaddrinfo(res);
//    close(fd);
//
//    return;
//}

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
    char **split;

    // Formulate the message with "NODES r" format
    snprintf(nodes, sizeof(nodes), "NODES %s", ring);

    // Call UDP_client to send the message and receive node_list
    // char *node_list = UDP_client(nodes);

    // Send the nodes message do the UDP Client
    UDP_client(nodes);

    char *node_list = buffer;
    succID == NULL;
    // Process the node_list
    // fprintf(stderr, "Received response:\n%s\n", node_list);

    // Extract the succesor id, ip and tcp

    char **bastos = __ft_split(node_list, '\n');

    if (bastos[1])
    {
        split = __ft_split(bastos[1], ' ');

        succID = split[0];
        succIP = split[1];
        succTCP = split[2];
    }

    fprintf(stderr, "SUCCID -> %s\n", succID);

    if (succID == NULL || !*succID)
    {

        fprintf(stderr, "PORT ENVIADO PARA SERVER -> %s\n", tcp);

        TCP_Server(tcp);

        // Establish a TCP connection to the successor node
        char tcp_message[128];
        snprintf(tcp_message, sizeof(tcp_message), "REG %s %s %s", id, ip, tcp);

        fprintf(stderr, "LOCAL REG: %s\n", tcp_message);

        // If node is empty then the node is its own successor

        fprintf(stderr, "PORT ENVIADO PARA CLIENT -> %s\n", tcp);

        TCP_Client(ip, tcp, tcp_message);
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

void Show_topology(char *id, char *succID, char *succIP, char *succTCP, char *succsuccID, char *succsuccIP, char *succsuccTCP, char *predID)
{
    printf("%s\n", id);
    printf("%s %s %s\n", succID, succIP, succTCP);
    printf("%s %s %s\n", succsuccID, succsuccIP, succsuccTCP);
    printf("%s\n", predID);
    return;
}

int Biggest_fd(int fd)
{
    int max_fd = my_fd;

    if (pred_fd > max_fd)
        max_fd = pred_fd;

    if (succ_fd > max_fd)
        max_fd = succ_fd;

    return max_fd;
}

void Read_buffer_tcp(int fd)
{

    char msg[128], ring[4], id[3], ip[13], tcp[6], command[3], input[100], succID[3], succIP[13], succTCP[6], succsuccID[3], succsuccIP[16], succsuccTCP[6], predID[3];

    char *token, line[50], local_succ[50], local_entry[50], local_pred[50], buffer[200], id_funcao[3], ip_funcao[20], port[20];
    int aux = 0;
    ssize_t n;

    n = read(fd, buffer, 200);
    if (n == -1)
        exit(1);
    write(1, "received: ", 10);
    write(1, buffer, n);

    token = strtok(buffer, "\n");
    strcpy(line, token);

    token = strtok(line, " ");
    if (strcmp(token, "ENTRY") == 0)
    {
        if (fd == succ_fd)
        {
            close(succ_fd);
            aux = 0;
            while (token = strtok(NULL, " "))
            {
                if (aux == 0)
                {
                    strcpy(id_funcao, token);
                    strcpy(succID, token);
                    aux++;
                }
                else if (aux == 1)
                {
                    strcpy(ip_funcao, token);
                    strcpy(succIP, token);
                    aux++;
                }
                else if (aux == 2)
                {
                    strcpy(port, token);
                    strcpy(succTCP, token);
                    aux++;
                }
            }

            strcpy(local_succ, "SUCC ");
            strcat(local_succ, id_funcao);
            strcat(local_succ, " ");
            strcat(local_succ, ip_funcao);
            strcat(local_succ, " ");
            strcat(local_succ, port);
            strcat(local_succ, "\n");
            strcat(local_succ, "\0");
            n = write(pred_fd, local_succ, n);
            if (n == -1)
                exit(1);

            strcpy(local_pred, "PRED ");
            strcat(local_pred, id);
            strcat(local_pred, "\n");
            strcat(local_pred, "\0");
            TCP_Client(succIP, succTCP, local_pred);
        }
        else
        {
            aux = 0;
            while (token = strtok(NULL, " "))
            {
                if (aux == 0)
                {
                    strcpy(id_funcao, token);
                    strcpy(predID, token);
                    aux++;
                }
                else if (aux == 1)
                {
                    strcpy(ip_funcao, token);
                    aux++;
                }
                else if (aux == 2)
                {
                    strcpy(port, token);
                    aux++;
                }
            }

            strcpy(local_succ, "SUCC ");
            strcat(local_succ, succsuccID);
            strcat(local_succ, " ");
            strcat(local_succ, succsuccIP);
            strcat(local_succ, " ");
            strcat(local_succ, succsuccTCP);
            strcat(local_succ, "\n");
            strcat(local_succ, "\0");
            n = write(fd, local_succ, n);
            if (n == -1)
                exit(1);

            if (pred_fd == -1)
            {
                strcpy(local_pred, "PRED ");
                strcat(local_pred, id);
                strcat(local_pred, "\n");
                strcat(local_pred, "\0");
                TCP_Client(ip_funcao, port, local_pred);
                pred_fd = fd;
            }
            else
            {
                strcpy(local_entry, "ENTRY ");
                strcat(local_entry, id_funcao);
                strcat(local_entry, " ");
                strcat(local_entry, ip_funcao);
                strcat(local_entry, " ");
                strcat(local_entry, port);
                strcat(local_entry, "\n");
                strcat(local_entry, "\0");
                n = write(pred_fd, local_entry, n);
                if (n == -1)
                    exit(1);
                pred_fd = fd;
            }
        }
    }
    else if (strcmp(token, "SUCC") == 0)
    {
        aux = 0;
        while (token = strtok(NULL, " "))
        {
            if (aux == 0)
            {
                strcpy(succsuccID, token);
                aux++;
            }
            else if (aux == 1)
            {
                strcpy(succsuccIP, token);
                aux++;
            }
            else if (aux == 2)
            {
                strcpy(succsuccTCP, token);
                aux++;
            }
        }
    }
    else if (strcmp(token, "PRED") == 0)
    {
        pred_fd = fd;

        token = strtok(NULL, "\n");
        strcpy(predID, token);
    }
    close(fd);
    return;
}

int Read_user_input()
{

    char msg[128], ring[4], id[3], ip[13], tcp[6], command[3], input[100], succID[3], succIP[13], succTCP[6], succsuccID[3], succsuccIP[16], succsuccTCP[6], predID[3];

    fgets(input, sizeof(input), stdin);

    sscanf(input, "%s", command);

    fprintf(stderr, "COMMAND -> %s\n", command);

    if (strcmp(command, "x") == 0)
    {
        printf("Exiting the program...\n");

        return 1;
    }
    else if (strcmp(command, "j") == 0)
    {
        sscanf(input, "%*s %s %s", ring, id);

        fprintf(stderr, "RING -> %s\n", ring);
        fprintf(stderr, "ID -> %s\n", id);

        if (strlen(ring) != 3)
        {
            printf("Anel inválido\n");
            return 0;
        }

        else if (strlen(id) != 2)
        {
            printf("No invalido\n");
            return 0;
        }

        else
        {
            // Hardcode do ip e tcp do no a registar
            char *ip = "127.0.0.1";
            char *tcp = "60001";

            join(ring, id, ip, tcp, succID, succIP, succTCP);

            return 0;
        }
    }

    else if (strcmp(command, "st") == 0)
    {
        Show_topology(id, succID, succIP, succTCP, succsuccID, succsuccIP, succsuccTCP, predID);

        return 0;
    }

    else
    {
        printf("Invalid command.\n");

        return 0;
    }
}

int main(int argc, char *argv[]) // Recebe os argumentos do terminal, argc - contador, agrv - cria uma lista tamanho agrc com os
{

    int my_fd, pred_fed, succ_fd;

    my_fd = socket(AF_INET, SOCK_STREAM, 0);
    pred_fd = socket(AF_INET, SOCK_STREAM, 0);
    succ_fd = socket(AF_INET, SOCK_STREAM, 0);

    char msg[128], ring[4], id[3], ip[13], tcp[6], command[3], input[100], succID[3], succIP[13], succTCP[6], succsuccID[3], succsuccIP[16], succsuccTCP[6], predID[3];

    // Verificar o numero de argumentos
    // if (argc < 3 || argc > 5)
    //    return 1;
    //
    // char *regIP;
    // char *regUDP;
    //
    // Verificar se o utilizador fornece regIP e regUDP
    // if (argv[3] == NULL)
    //{
    //    regIP = strdup("193.136.138.142"); // Copia e aloca memória
    //    regUDP = strdup("60001");
    //}
    // else
    //{
    //    regIP = argv[3];
    //    regUDP = argv[4];
    //}

    // while (1)
    // {
    //     printf("\nWrite the wanted command using the correct format:\n");
    //
    //     fgets(input, sizeof(input), stdin);
    //
    //     sscanf(input, "%s", command);
    //
    //     fprintf(stderr, "COMMAND -> %s\n", command);
    //
    //     if (strcmp(command, "x") == 0)
    //     {
    //         printf("Exiting the program...\n");
    //         break;
    //     }
    //     else if (strcmp(command, "j") == 0)
    //     {
    //         sscanf(input, "%*s %s %s", ring, id);
    //
    //         fprintf(stderr, "RING -> %s\n", ring);
    //         fprintf(stderr, "ID -> %s\n", id);
    //
    //         if (strlen(ring) != 3)
    //         {
    //             printf("Anel inválido");
    //         }
    //
    //         else if (strlen(id) != 2)
    //         {
    //             printf("No invalido");
    //         }
    //
    //         else
    //         {
    //             // Hardcode do ip e tcp do no a registar
    //             char *ip = "127.0.0.1";
    //             char *tcp = "60001";
    //
    //             join(ring, id, ip, tcp, succID, succIP, succTCP);
    //         }
    //
    //     }
    //
    //     else if (strcmp(command, "st") == 0)
    //     {
    //         Show_topology(id, succID, succIP, succTCP, succsuccID, succsuccIP, succsuccTCP, predID);
    //     }
    //
    //     else
    //     {
    //         printf("Invalid command.\n");
    //     }
    // }

    // Main loop

    // ALTERAR

    while (1)
    {
        // Set up the file descriptor set
        FD_ZERO(&rfds);
        FD_SET(0, &rfds);       // Add stdin to the set
        FD_SET(my_fd, &rfds);   // Add my_fd to the set
        FD_SET(succ_fd, &rfds); // Add succ_fd to the set
        FD_SET(pred_fd, &rfds); // Add pred_fd to the set

        // Find the maximum file descriptor
        int max_fd = Biggest_fd(fd);

        // Set the timeout
        struct timeval tv;
        tv.tv_sec = 10; // 10 seconds timeout
        tv.tv_usec = 0;

        // Wait for input on any file descriptor in the set
        fprintf(stderr, "Write the wanted command using the correct format:\n");
        int counter = select(1, &rfds, NULL, NULL, &tv); // Monitoring only stdin

        fprintf(stderr, "COUNTER -> %d\n", counter);

        if (counter == -1)
        {
            perror("select");
            exit(EXIT_FAILURE);
        }
        // else if (counter > 0)
        // {
        // Check if input is available on stdin
        if (FD_ISSET(0, &rfds))
        {
            // Input is available on stdin, read it
            if (Read_user_input() != 0)
            {
                break; // Exit the loop if Read_user_input() returns non-zero
            }
        }
        if (FD_ISSET(succ_fd, &rfds))
        {
            // Handle data on the successor socket
            Read_buffer_tcp(succ_fd);
        }
        if (FD_ISSET(pred_fd, &rfds))
        {
            // Handle data on the predecessor socket
            Read_buffer_tcp(pred_fd);
        }
        if (FD_ISSET(my_fd, &rfds))
        {
            // Handle data on the server socket
            addrlen = sizeof(addr);
            if ((newfd = accept(my_fd, (struct sockaddr *)&addr, &addrlen)) == -1)
            {
                perror("accept");
                exit(1);
            }
            Read_buffer_tcp(newfd);
        }
        //}
        else
        {
            // Timeout occurred
            printf("TIMOUT! No input received within 10 seconds.\n");
        }
    }

    // free(regIP);
    // free(regUDP);

    return 0;
}
