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
char nodes_mesg[10];

char my_ring[4], my_id[3], ip[16], tcp[6], succID[3], succIP[16], succTCP[6], succsuccID[3], succsuccIP[16], succsuccTCP[6], predID[3], index_linha[16][3], index_coluna[16][3], encaminhamento[16][16][50], caminhos[16][50], expedicao[16][2];
int succ_fd = -1, pred_fd = -1, my_fd = -1, broke_connection = -1, index_coluna_max = -1, index_linha_max = -1, index_cost[16];

void UDP_Client(char *msg)
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

int Succ_from_Nodeslist(char *buffer)
{
    char *line_token, *id_token, line[50];
    int aux_out = 0, aux_read = 0;

    line_token = strtok(buffer, "\n");
    if ((line_token = strtok(NULL, "\n")) != NULL)
    {
        strcpy(line, line_token);
        id_token = strtok(line, " ");
        strcpy(succID, id_token);
        while (id_token = strtok(NULL, " "))
        {
            if (aux_read == 0)
            {
                strcpy(succIP, id_token);
                aux_read++;
            }
            else if (aux_read == 1)
            {
                strcpy(succTCP, id_token);
                aux_read++;
                return 0;
            }
        }
        return 0;
    }

    return 1;
}

int Read_buffer_Nodeslist(char *buffer)
{
    char *line_token, *id_token, id[3], ip[20], port[20], line[50];
    int aux = 0, l = 0, c = 0;
    char id_list[11] = "0123456789";

    line_token = strtok(buffer, "\n");
    do
    {
        if (aux == 0)
        {
            aux++;
        }
        else
        {
            strcpy(line, line_token);
            id_token = strtok(line, " ");
            if (strcmp(id, id_token) == 0)
            {
                id[1] = id_list[l];
                l++;
                if (l == 9)
                {
                    l = 0;
                    c++;
                }
                id[0] = id_list[c];
                return 1;
            }
        }
    } while (line_token = strtok(NULL, "\n"));
    return 0;
}

Node *Create_Node(id, ip, tcp, succID, succIP, succTCP, succsuccID, succsuccIP, succsuccTCP, predID)
{
    // Allocate memory for the Node struct
    Node *Node_reg = malloc(sizeof(Node));

    Node_reg->node_id = id;
    Node_reg->node_ip = ip;
    Node_reg->node_port = tcp;
    Node_reg->succ_id = succID;
    Node_reg->succ_ip = succIP;
    Node_reg->succ_port = succTCP;
    Node_reg->succsucc_id = succsuccID;
    Node_reg->succsucc_ip = succsuccIP;
    Node_reg->succsucc_port = succsuccTCP;
    Node_reg->pred_id = predID;

    return Node_reg;
}

Node *Save_Node(Node **Ring, Node *New_Node)
{
    // We have to go trough all the positions and find the next empty one
    for (size_t i = 0; i < 15; i++)
    {
        if (Ring[i] == NULL)
        {
            Ring[i] = New_Node;
        }

    }
    
}

int join(char *ring, char *id, char *ip, char *tcp, char *succID, char *succIP, char *succTCP, char *succsuccID, char *succsuccIP, char *succsuccTCP, char *predID, Node **Ring)
{
    char **split;

    // Formulate the message with "NODES r" format
    snprintf(nodes_mesg, sizeof(nodes_mesg), "NODES %s", ring);

    // Send the nodes message do the UDP Client
    UDP_Client(nodes_mesg);

    char *received_list = buffer;
    succID == NULL;

    // Extract the succesor id, ip and tcp
    char **split_mesg = __ft_split(received_list, '\n');

    split = __ft_split(split_mesg[1], ' ');

    succID = split[0];

    // If there is no succID -> ring has no nodes
    if (succID == NULL || !*succID)
    {
        // The node becomes its own successor, succsuccessor and predeccessor
        strcpy(succID, id);
        strcpy(succIP, ip);
        strcpy(succTCP, tcp);
        strcpy(predID, id);
        strcpy(succsuccID, id);
        strcpy(succsuccIP, ip);
        strcpy(succsuccTCP, tcp);
    }

    if (succID != NULL)
    {
        // Check if the joining ID already exists in the ring
        if (id_exists(received_list, id))
        {
            fprintf(stderr, "ID %s already exists in the ring. Generating a new ID...\n", id);

            // Generate a new ID
            srand(time(NULL));
            int new_id = (rand() % 99) + 1; // Generate a random number between 1 and 99

            snprintf(id, 3, "%02d", new_id); // Ensure id has enough space

            fprintf(stderr, "GENERATED ID: %s\n", id);

            // Now, we need to recheck if the new ID exists in the node list
            // If it does, repeat the process until we find a unique ID
            while (id_exists(received_list, id))
            {
                fprintf(stderr, "New ID %s already exists in the ring. Generating another new ID...\n", id);

                new_id = (rand() % 99) + 1;
                snprintf(id, 3, "%02d", new_id);
            }

            fprintf(stderr, "Final ID: %s\n", id);

            // Extract successor, succsuccessor and predeccessor info
            for (size_t i = 1; split_mesg[i]; i++)
            {
                
            }
        }

        TCP_Server(tcp);

        // Write ENTRY message
        char entry_message[128];

        snprintf(entry_message, sizeof(entry_message), "ENTRY %s %s %s", id, ip, tcp);

        if (strcmp(succID, id) != 0)
        {
            TCP_Client(succIP, succTCP, entry_message);

            char route_message[128];

            snprintf(route_message, sizeof(route_message), "ROUTE %s %s %s-%s-%s", id, succID, id, id, id);

            strcpy(route_message, "ROUTE ");
            strcat(route_message, id);
            strcat(route_message, " ");
            strcat(route_message, id);
            strcat(route_message, " ");
            strcat(route_message, id);
            strcat(route_message, "\n");
            strcat(route_message, "\0");
            n = write(succ_fd, route_message, strlen(route_message));
            if (n == -1)
                exit(1);
            /*index_coluna_max++;
            strcpy(index_coluna[index_coluna_max],succ_id);
            index_linha_max++;
            strcpy(index_linha[index_linha_max],succ_id);*/
        }
    }

    char reg_message[128];
    snprintf(reg_message, sizeof(reg_message), "REG %s %s %s %s", ring, id, ip, tcp);

    fprintf(stderr, "MESG REG: %s\n", reg_message);

    UDP_Client(reg_message);

    // Create the register node in the node struct
    Node *New_Node = Create_Node(id, ip, tcp, succID, succIP, succTCP, succsuccID, succsuccIP, succsuccTCP, predID);

    // Save the new node in the Ring
    Save_Node(Ring, New_Node);

    return 0;
}

void Show_topology(char *id, char *succID, char *succIP, char *succTCP, char *succsuccID, char *succsuccIP, char *succsuccTCP, char *predID)
{
    printf("My ID: %s\n", id);
    printf("Successor info: %s %s %s\n", succID, succIP, succTCP);
    printf("Successorsuccessor info: %s %s %s\n", succsuccID, succsuccIP, succsuccTCP);
    printf("Predeccesser info: %s\n", predID);
    return;
}

int Biggest_fd(int fd)
{
    fd = -1;

    fd = my_fd;
    if (succ_fd > fd)
        fd = succ_fd;
    if (pred_fd > fd)
        fd = pred_fd;

    return fd;
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

// int Read_user_input()
//{
//
//     char msg[128], ring[4], id[3], ip[13], tcp[6], command[3], input[100], succID[3], succIP[13], succTCP[6], succsuccID[3], succsuccIP[16], succsuccTCP[6], predID[3];
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
//
//         return 1;
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
//             printf("Anel inválido\n");
//             return 0;
//         }
//
//         else if (strlen(id) != 2)
//         {
//             printf("No invalido\n");
//             return 0;
//         }
//
//         else
//         {
//             // Hardcode do ip e tcp do no a registar
//             char *ip = "127.0.0.1";
//             char *tcp = "60001";
//
//             fprintf(stderr, "ANTES DO JOIN\n");
//
//             join(ring, id, ip, tcp, succID, succIP, succTCP);
//
//             return 0;
//         }
//     }
//
//     else if (strcmp(command, "st") == 0)
//     {
//         Show_topology(id, succID, succIP, succTCP, succsuccID, succsuccIP, succsuccTCP, predID);
//
//         return 0;
//     }
//
//     else
//     {
//         printf("Invalid command.\n");
//
//         return 0;
//     }
// }

void leave(char *ring, char *id)
{
    char local_unreg[13], unreg_buffer[8], local_entry[50];
    ssize_t n;

    strcpy(local_unreg, "UNREG ");
    strcat(local_unreg, ring);
    strcat(local_unreg, " ");
    strcat(local_unreg, id);
    strcat(local_unreg, "\0");

    // UDP_Client(local_unreg, unreg_buffer);
    UDP_Client(local_unreg);

    close(succ_fd);
    succ_fd = -1;
    close(pred_fd);
    pred_fd = -1;
    close(my_fd);
    my_fd = -1;

    memset(my_ring, 0, strlen(my_ring));
    memset(my_id, 0, strlen(my_id));
    memset(predID, 0, strlen(predID));
    memset(succID, 0, strlen(succID));
    memset(succsuccID, 0, strlen(succsuccID));
    memset(succIP, 0, strlen(succIP));
    memset(succsuccIP, 0, strlen(succsuccIP));
    memset(succTCP, 0, strlen(succTCP));
    memset(succsuccTCP, 0, strlen(succsuccTCP));

    return;
}

// Function returns Node pointer


int main(int argc, char *argv[]) // Recebe os argumentos do terminal, argc - contador, agrv - cria uma lista tamanho agrc com os
{
    // Ring is a set of 16 nodes (maximum)
    Node **Ring = malloc(sizeof(Node*) * 16);

    char *token, buffer[200], ring[4], id[3], command[3];
    int fd, counter, i, j;
    fd_set rfds;
    int newfd;
    socklen_t addrlen;
    struct sockaddr_in addr;

    strcpy(ip, argv[1]);
    strcpy(tcp, argv[2]);

    for (i = 0; i <= 16; i++)
    {
        sprintf(index_coluna[i], "%d", -1);
        sprintf(index_linha[i], "%d", -1);
        index_cost[i] = 100;
        for (j = 0; j <= 16; j++)
        {
            sprintf(encaminhamento[i][j], "%d", -1);
        }
    }

    for (i = 0; i <= 16; i++)
    {
        sprintf(expedicao[i], "%d", -1);
        sprintf(caminhos[i], "%d", -1);
    }

    printf("Write the wanted command using the correct format:\n");
    scanf("%s %s %s", command, ring, id);
    // strcpy(my_id,id);
    // strcpy(my_ring,ring);

    join(ring, id, ip, tcp, succID, succIP, succTCP, succsuccID, succsuccIP, succsuccTCP, predID);

    while (1)
    {
        fd = Biggest_fd(fd);
        printf("\n");
        FD_ZERO(&rfds);         /* remover todos os descritores do conjunto */
        FD_SET(0, &rfds);       /* adicionar o descritor 0 (stdin) ao conjunto */
        FD_SET(my_fd, &rfds);   /* adicionar o descritor fd (socket TCP) ao conjunto */
        FD_SET(pred_fd, &rfds); /* adicionar o descritor 0 (stdin) ao conjunto */
        FD_SET(succ_fd, &rfds); /* adicionar o descritor fd (socket TCP) ao conjunto */

        counter = select(fd + 1, &rfds, (fd_set *)NULL, (fd_set *)NULL, (struct timeval *)NULL);
        if (counter == -1)
            exit(1);

        if (FD_ISSET(0, &rfds) != 0)
        {
            memset(buffer, 0, 200);
            fgets(buffer, 200, stdin);

            if (buffer[0] == 'j')
            {
                token = strtok(buffer, " ");
                token = strtok(NULL, " ");
                strcpy(my_ring, token);
                token = strtok(NULL, " ");
                strcpy(my_id, token);
                join(ring, id, ip, tcp, succID, succIP, succTCP, succsuccID, succsuccIP, succsuccTCP, predID);
            }
            else if (buffer[0] == 'l')
            {
                leave(ring, id);
            }
            else if (buffer[0] == 's')
            {
                if (buffer[1] == 't')
                {
                    Show_topology(id, succID, succIP, succTCP, succsuccID, succsuccIP, succsuccTCP, predID);
                }
                else if (buffer[1] == 'r')
                {
                }
                else if (buffer[1] == 'p')
                {
                }
                else if (buffer[1] == 'f')
                {
                }
                else
                {
                    token = strtok(buffer, " ");
                    token = strtok(NULL, " ");
                    if (strcmp(token, "topology") == 0)
                    {
                        Show_topology(id, succID, succIP, succTCP, succsuccID, succsuccIP, succsuccTCP, predID);
                    }
                    else if (strcmp(token, "routing") == 0)
                    {
                    }
                    else if (strcmp(token, "path") == 0)
                    {
                    }
                    else if (strcmp(token, "fowarding") == 0)
                    {
                    }
                }
            }
            else if (buffer[0] == 'x' || buffer[0] == 'e')
            {
                return 0;
            }
            else if (buffer[0] == 'm')
            {
            }
        }
        if (FD_ISSET(my_fd, &rfds) != 0)
        {
            addrlen = sizeof(addr);
            if ((newfd = accept(my_fd, (struct sockaddr *)&addr, &addrlen)) == -1)
                exit(1);
            Read_buffer_tcp(newfd);
        }
        if (FD_ISSET(pred_fd, &rfds) != 0)
        {
            Read_buffer_tcp(pred_fd);
        }
        if (FD_ISSET(succ_fd, &rfds) != 0)
        {
            Read_buffer_tcp(succ_fd);
        }
        // printf("new cycle");
    }

    // return 0;
}
