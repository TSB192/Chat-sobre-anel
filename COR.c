#include "COR.h"

// int errcode, udp_fd, newfd, counter;
// int fd;
// int succ_fd;
// int pred_fd;
// int my_fd;
fd_set rfds;
ssize_t n;
socklen_t addrlen;
struct addrinfo hints, *res;
struct sockaddr_in addr;
char buffer[1028];
char udp_buffer[1028];
char nodes_mesg[10];

char ip[16], tcp[6], succID[3], succIP[16], succTCP[6], succsuccID[3], succsuccIP[16], succsuccTCP[6], predID[3];
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

Node *Create_Node(char *id, char *ip, char *tcp)
{
    // Allocate memory for the Node struct
    Node *Node_reg = malloc(sizeof(Node));

    Node_reg->node_id = strdup(id);
    Node_reg->node_ip = strdup(ip);
    Node_reg->node_port = strdup(tcp);
    Node_reg->succ_id = strdup(id);
    Node_reg->succ_ip = strdup(ip);
    Node_reg->succ_port = strdup(tcp);
    Node_reg->succsucc_id = strdup(id);
    Node_reg->succsucc_ip = strdup(ip);
    Node_reg->succsucc_port = strdup(tcp);
    Node_reg->pred_id = strdup(id);

    return Node_reg;
}

Node *Save_Node(Node **Ring, Node *New_Node)
{
    int j;

    // We have to go trough all the positions and find the next empty one
    for (int i = 0; i < 16; i++)
    {
        if (Ring[i] == NULL)
        {
            // Saves the new node in the ring
            Ring[i] = New_Node;

            Ring[i]->succ_id = Ring[0]->node_id;
            Ring[i]->succ_ip = Ring[0]->node_ip;
            Ring[i]->succ_port = Ring[0]->node_port;

            Ring[i]->succsucc_id = Ring[0]->succ_id;
            Ring[i]->succsucc_ip = Ring[0]->succ_ip;
            Ring[i]->succsucc_port = Ring[0]->succ_port;

            if (i != 0)
            {
                // Update the successor --> From the second node added onwards, the succ of the node previously added is the always the node that was just added
                Ring[i - 1]->succsucc_id = Ring[i]->succ_id;
                Ring[i - 1]->succsucc_ip = Ring[i]->succ_ip;
                Ring[i - 1]->succsucc_port = Ring[i]->succ_port;
            }

            j = i;

            break;
        }
    }

    // Update the predecessor of the first node in the ring
    Ring[0]->pred_id = New_Node->node_id;

    if (j != 0)
    {
        // Update the successor --> From the second node added onwards, the succ of the node previously added is the always the node that was just added
        Ring[j - 1]->succ_id = Ring[j]->node_id;
        Ring[j - 1]->succ_ip = Ring[j]->node_ip;
        Ring[j - 1]->succ_port = Ring[j]->node_port;

        // Update the predeccessor --> From the second node added onwards, the pred of the node is always the node previously added
        Ring[j]->pred_id = Ring[j - 1]->node_id;
    }

    if (j > 1)
    {
        // Update the second successor --> same logic, but with 2 node interval
        Ring[j - 2]->succsucc_id = Ring[j - 1]->succ_id;
        Ring[j - 2]->succsucc_ip = Ring[j - 1]->succ_ip;
        Ring[j - 2]->succsucc_port = Ring[j - 1]->succ_port;
    }
}

Node *Fill_Ring(Node **Ring, char **received_list)
{
    for (int i = 0; i < 16; i++)
    {
        if (Ring[i])
        {
            // Clean the ring before updating it
            free(Ring[i]);
            Ring[i] = NULL;
        }
    }

    for (int i = 0; received_list[i]; i++)
    {
        // Add pre-existing nodes to the ring
        char **info = __ft_split(received_list[i], ' ');
        Node *New_Node = Create_Node(info[0], info[1], info[2]);

        Save_Node(Ring, New_Node);
    }
}

Node *Remove_Node(Node **Ring)
{
    for (int i = 0; i < 16; i++)
    {
        if (Ring[i + 1] == NULL)
        {
            if (i == 0)
            {
                Ring[i] = NULL;

                break;
            }

            // Update the suc for the previous node
            Ring[i - 1]->succ_id = Ring[i]->succ_id;
            Ring[i - 1]->succ_ip = Ring[i]->succ_ip;
            Ring[i - 1]->succ_port = Ring[i]->succ_port;

            // Update the sucsuc for the previous node
            Ring[i - 1]->succsucc_id = Ring[i]->succsucc_id;
            Ring[i - 1]->succsucc_ip = Ring[i]->succsucc_ip;
            Ring[i - 1]->succsucc_port = Ring[i]->succsucc_port;

            // Update the pred for the previous node
            Ring[0]->pred_id = Ring[i - 1]->node_id;

            free(Ring[i]);
            Ring[i] = NULL;

            break;
        }
    }
}

// Create route table
char *Create_Table_Route(Node **Ring, char *dest_id, int flag)
{
    int origin;
    char *encaminhamento_right = calloc(32, 1);
    char *encaminhamento_left = calloc(32, 1);

    // char *encaminhamento_chord = malloc(32);

    for (int i = 0; i < 16; i++)
    {
        // Check last node added to the ring
        if (Ring[i + 1] == NULL)
        {
            origin = i;
            break;
        }
    }

    // Add the node id of the origin to the array
    strcat(encaminhamento_right, Ring[origin]->node_id);
    strcat(encaminhamento_left, Ring[origin]->node_id);
    // strcat(encaminhamento_chord, Ring[origin]->node_id);

    // Coluna de encaminhamento do caminho para a direita
    for (int i = 0; i < origin; i++)
    {

        // String inteira menos o valor de destino
        if (strncmp(Ring[i]->node_id, dest_id, 2))
        {

            strcat(encaminhamento_right, "-");
            strcat(encaminhamento_right, Ring[i]->node_id);
        }
        // Add destiny value
        else
        {
            strcat(encaminhamento_right, "-");
            strcat(encaminhamento_right, dest_id);
            break;
        }
    }
    // Coluna de encaminhamento do caminho para a esquerda
    for (int i = 1; i <= origin; i++)
    {
        // String inteira menos o valor de destino
        if (strncmp(Ring[origin - i]->node_id, dest_id, 2))
        {
            strcat(encaminhamento_left, "-");
            strcat(encaminhamento_left, Ring[origin - i]->node_id);
        }
        // Add destiny value
        else
        {
            strcat(encaminhamento_left, "-");
            strcat(encaminhamento_left, dest_id);
            break;
        }
    }

    // Coluna de encaminhamento do caminho para a corda
    // for (int i = 0; i < origin ; i++)
    // {
    //     // String inteira menos o valor de destino
    //     if (Ring[i]->chord_id != dest_id)
    //     {
    //         strcat(encaminhamento_chord, "-");
    //         strcat(encaminhamento_chord, Ring[i]->node_id);
    //     }
    //     // Add destiny value
    //     else
    //     {
    //         strcat(encaminhamento_chord, "-");
    //         strcat(encaminhamento_chord, dest_id);
    //         strcat(encaminhamento_chord, "\n");
    //         break;
    //     }
    // }

    if (flag)
    {
        printf("Caminhos direita %s\n", encaminhamento_right);
        printf("Caminhos esquerda %s\n", encaminhamento_left);
    }

    // Check which string is shorter, and returns it
    return (strlen(encaminhamento_left) > strlen(encaminhamento_right) ? encaminhamento_right : encaminhamento_left);
}

// Create shortest path table
char *Create_Table_Path(Node **Ring, char *dest_id, int flag)
{
    char *path = Create_Table_Route(Ring, dest_id, 0);

    if (flag)
    {
        printf("%s", path);
    }
    else
        printf("%c%c", path[3], path[4]);
}

// Create fowarding table
char *Create_Table_Foward(Node **Ring)
{
    for (int i = 0; i < 16; i++)
    {
        if (Ring[i])
        {
            printf("%s ", Ring[i]->node_id);
            
            // When the next position is NULL
            if (!Ring[i+1])
            {
                printf("-\n");
                break;
            }
            
            Create_Table_Path(Ring, Ring[i]->node_id, 0);
            printf("\n");
        }

    }
}

// void Full_Table_Route(Node **Ring, )
// {

// }

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

    Fill_Ring(Ring, &split_mesg[1]);

    if (split_mesg[1])
    {
        split = __ft_split(split_mesg[1], ' ');

        succID = split[0];
        succIP = split[1];
        succTCP = split[2];
    }

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

        // Create the register node in the node struct
        Node *New_Node = Create_Node(id, ip, tcp);

        // Save the new node in the Ring
        Save_Node(Ring, New_Node);

        // Create_Table(Ring);
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
        }

        // Create the register node in the node struct
        Node *New_Node = Create_Node(id, ip, tcp);

        // Save the new node in the Ring
        Save_Node(Ring, New_Node);

        TCP_Server(tcp);

        fprintf(stderr, "RING -> %s\n", ring);

        // Write ENTRY message
        char entry_message[128];

        snprintf(entry_message, sizeof(entry_message), "ENTRY %s %s %s", id, ip, tcp);

        // TCP_Client(ip, tcp, entry_message);

        // Create_Table(Ring);
    }

    char reg_message[128];
    snprintf(reg_message, sizeof(reg_message), "REG %s %s %s %s", ring, id, ip, tcp);

    UDP_Client(reg_message);

    // Create the register node in the node struct
    // Node *New_Node = Create_Node(id, ip, tcp);

    // Save the new node in the Ring
    // Save_Node(Ring, New_Node);

    // Check_Routes(Ring);

    return 0;
}

void Show_topology(char *id, char *succID, char *succIP, char *succTCP, char *succsuccID, char *succsuccIP, char *succsuccTCP, char *predID, Node **Ring)
{

    UDP_Client("NODES 092");

    char *received_list = buffer;

    char **split_mesg = __ft_split(received_list, '\n');

    Fill_Ring(Ring, &split_mesg[1]);

    for (size_t i = 0; Ring[i]; i++)
    {
        printf("Node info: %s\n", Ring[i]->node_id);
        printf("Sucessor info: %s %s %s\n", Ring[i]->succ_id, Ring[i]->succ_ip, Ring[i]->succ_port);
        printf("Successor successor info: %s %s %s\n", Ring[i]->succsucc_id, Ring[i]->succsucc_ip, Ring[i]->succsucc_port);
        printf("Predecestor info: %s\n", Ring[i]->pred_id);
    }
}

int Biggest_fd(int fd)
{
    // Initialize fd to the first file descriptor
    fd = my_fd;

    // Update fd if succ_fd is larger
    if (succ_fd > fd)
        fd = succ_fd;

    // Update fd if pred_fd is larger
    if (pred_fd > fd)
        fd = pred_fd;

    return fd;
}

void Read_buffer_tcp(int fd, Node **Ring)
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
        // Split the tokenized received message by space ' '
        // char **split_mesg = __ft_split(token, ' ');

        fprintf(stderr, "FD -> %d\n", fd);

        fprintf(stderr, "SUCC FD -> %d\n", succ_fd);

        if (fd == succ_fd)
        {
            close(succ_fd);
            aux = 0;

            while (token = strtok(NULL, " "))
            {
                fprintf(stderr, "\nTOKEN WHEN FD == SECC_FD-> %s\n", token);

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
                fprintf(stderr, "\nTOKEN NO ELSE -> %s\n", token);

                // First cycle
                if (aux == 0)
                {
                    strcpy(id_funcao, token);
                    strcpy(predID, token);
                    aux++;
                }
                // Second cycle
                else if (aux == 1)
                {
                    strcpy(ip_funcao, token);
                    aux++;
                }
                // Third cyle
                else if (aux == 2)
                {
                    strcpy(port, token);
                    aux++;
                }
            }

            // snprintf(local_succ, "SUCC %s %s")

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

void leave(char *ring, char *id, Node **Ring)
{
    char local_unreg[13], unreg_buffer[8], local_entry[50];

    char unreg_message[128];
    snprintf(unreg_message, sizeof(unreg_message), "UNREG %s %s", ring, id);

    UDP_Client(unreg_message);

    close(succ_fd);
    succ_fd = -1;
    close(pred_fd);
    pred_fd = -1;
    close(my_fd);
    my_fd = -1;

    Remove_Node(Ring);

    return;
}

int main(int argc, char *argv[]) // Recebe os argumentos do terminal, argc - contador, agrv - cria uma lista tamanho agrc com os
{
    // Ring is a set of 16 nodes (maximum) -- allocated 16 slots of memory
    // Node **Ring = malloc(sizeof(Node *) * 16);
    Node **Ring = calloc(16, sizeof(Node *));

    char *ring = malloc(4);
    char *id = malloc(3);
    char *command = malloc(2);
    char *dest_id = malloc(3);

    char buffer[200];

    int fd, counter, i, j;
    fd_set rfds;
    int newfd;
    socklen_t addrlen;
    struct sockaddr_in addr;

    strcpy(ip, argv[1]);
    strcpy(tcp, argv[2]);

    printf("\nWrite the wanted command using the correct format:\n");
    scanf("%s %s %s", command, ring, id);
    // strcpy(my_id,id);
    // strcpy(my_ring,ring);

    join(ring, id, ip, tcp, succID, succIP, succTCP, succsuccID, succsuccIP, succsuccTCP, predID, Ring);

    while (1)
    {
        fd = Biggest_fd(fd);
        printf("\n");
        FD_ZERO(&rfds);         /* remover todos os descritores do conjunto */
        FD_SET(0, &rfds);       /* adicionar o descritor 0 (stdin) ao conjunto */
        FD_SET(my_fd, &rfds);   /* adicionar o descritor fd (socket TCP) ao conjunto */
        FD_SET(pred_fd, &rfds); /* adicionar o descritor 0 (stdin) ao conjunto */
        FD_SET(succ_fd, &rfds); /* adicionar o descritor fd (socket TCP) ao conjunto */

        counter = select(fd + 1, &rfds, (fd_set *)NULL, (fd_set *)NULL, NULL);
        if (counter == -1)
            exit(1);

        if (FD_ISSET(0, &rfds) != 0)
        {
            memset(buffer, 0, 200);
            fgets(buffer, 200, stdin);

            if (buffer[0] == 'j')
            {
                char **tokens = __ft_split(buffer, ' ');
                ring = tokens[1];
                id = tokens[2];
                join(ring, id, ip, tcp, succID, succIP, succTCP, succsuccID, succsuccIP, succsuccTCP, predID, Ring);
            }
            else if (buffer[0] == 'l')
            {
                fprintf(stderr, "ID -> %s\n", id);
                leave(ring, id, Ring);
            }
            else if (buffer[0] == 's')
            {
                char **tokens = __ft_split(buffer, ' ');
                dest_id = tokens[1];

                if (buffer[1] == 't')
                {
                    Show_topology(id, succID, succIP, succTCP, succsuccID, succsuccIP, succsuccTCP, predID, Ring);
                }
                else if (buffer[1] == 'r')
                {
                    Create_Table_Route(Ring, dest_id, 1);
                }
                else if (buffer[1] == 'p')
                {
                    Create_Table_Path(Ring, dest_id, 1);
                }
                else if (buffer[1] == 'f')
                {
                    Create_Table_Foward(Ring);
                }
                // else
                // {
                //     token = strtok(buffer, " ");
                //     token = strtok(NULL, " ");
                //     if (strcmp(token, "topology") == 0)
                //     {
                //         Show_topology(id, succID, succIP, succTCP, succsuccID, succsuccIP, succsuccTCP, predID, Ring);
                //     }
                //     else if (strcmp(token, "routing") == 0)
                //     {
                //     }
                //     else if (strcmp(token, "path") == 0)
                //     {
                //     }
                //     else if (strcmp(token, "fowarding") == 0)
                //     {
                //     }
                // }
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

            fprintf(stderr, "NEW_FD -> %d\n", newfd);
            Read_buffer_tcp(newfd, Ring);
        }
        if (FD_ISSET(pred_fd, &rfds) != 0)
        {
            Read_buffer_tcp(pred_fd, Ring);
        }
        if (FD_ISSET(succ_fd, &rfds) != 0)
        {
            Read_buffer_tcp(succ_fd, Ring);
        }
        // printf("new cycle");
    }

    return 0;
}
