#include "FUNCS.h"

void Create_Node()
{
    // Allocate memory for the Node struct
    My_Node = calloc(1, sizeof(Node));

    My_Node->chord_id = calloc(1, 3);
    My_Node->node_id = calloc(1, 3);
    My_Node->succ_id = calloc(1, 3);
    My_Node->succsucc_id = calloc(1, 3);
    My_Node->pred_id = calloc(1, 3);

    My_Node->chord_ip = calloc(1, 17);
    My_Node->node_ip = calloc(1, 17);
    My_Node->succ_ip = calloc(1, 17);
    My_Node->succsucc_ip = calloc(1, 17);

    My_Node->chord_port = calloc(1, 6);
    My_Node->node_port = calloc(1, 6);
    My_Node->succ_port = calloc(1, 6);
    My_Node->succsucc_port = calloc(1, 6);

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

void UDP_Client(char *msg, char *buffer)
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

    buffer[n] = 0;
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
    succ_fd = fd;

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

    freeaddrinfo(res);

    return;
}

Route* findRoute(char* id) {
    Route* current = Routes;
    while (current != NULL) {
        if (strcmp(current->id, id) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL; // route not found
}

void updateRoute(char* id, char* rpred, char* rsucc) {
    Route* route = findRoute(id);
    if (route != NULL) {
        strcpy(route->route_pred, rpred);
        strcpy(route->route_succ, rsucc);
    }
}

void Read_buffer_tcp(int fd)
{
    char *token, line[50], buffer[200], id[3], ip[20], port[20], origem[3], destino[3], new_path[50];
    int aux = 0, cost = 0, new_cost = 0, i, j, found = -1, position;
    ssize_t n;

    char *local_succ = calloc(50, 1);
    char *local_entry = calloc(50, 1);
    char *local_pred = calloc(50, 1);

    n = read(fd, buffer, 200);
    if (n == -1)
        exit(1);
    char **received = __ft_split(buffer, '\n');
    char **tokens = __ft_split(received[0], ' ');
    if (n == 0)
    {
        if (fd == succ_fd)
        {
            close(succ_fd);
            succ_fd = -1;

            strcpy(My_Node->succ_id, My_Node->succsucc_id);
            strcpy(My_Node->succ_ip, My_Node->succsucc_ip);
            strcpy(My_Node->succ_port, My_Node->succsucc_port);

            sprintf(local_succ, "SUCC %s %s %s\n", My_Node->succ_id, My_Node->succ_ip, My_Node->succ_port);
            n = write(pred_fd, local_succ, strlen(local_succ));
            if (n == -1)
                exit(1);

            sprintf(local_pred, "PRED %s\n", My_Node->node_id);

            TCP_Client(My_Node->succ_ip, My_Node->succ_port, local_pred);
        }
        else if (fd == pred_fd)
        {
            close(pred_fd);
            pred_fd = -1;
            broke_connection = 0;
        }
    }
    else
    {
        printf("%s\n", received[0]);
        if (!strcmp(tokens[0], "ENTRY"))
        {
            if (fd == succ_fd)
            {
                
                strcpy(My_Node->succsucc_id, My_Node->succ_id);
                strcpy(My_Node->succsucc_ip, My_Node->succ_ip);
                strcpy(My_Node->succsucc_port, My_Node->succ_port);
                close(succ_fd);
                succ_fd = -1;
    
                strcpy(My_Node->succ_id, tokens[1]);
                strcpy(My_Node->succ_ip, tokens[2]);
                strncpy(My_Node->succ_port, tokens[3], 5);

                sprintf(local_succ, "SUCC %s %s %s\n", tokens[1], tokens[2], My_Node->succ_port);

                n = write(pred_fd, local_succ, strlen(local_succ));
                if (n == -1)
                    exit(1);

                sprintf(local_pred, "PRED %s\n", My_Node->node_id);
                TCP_Client(My_Node->succ_ip, My_Node->succ_port, local_pred);
            }
            else
            {
                strcpy(My_Node->pred_id, tokens[1]);
                strcpy(My_Node->node_ip, tokens[2]);
                strncpy(My_Node->node_port, tokens[3], 5);
                if (pred_fd == -1)
                {

                    strcpy(My_Node->succ_id, tokens[1]);
                    strcpy(My_Node->succ_ip, tokens[2]);
                    strncpy(My_Node->succ_port, tokens[3], 5);
                    sprintf(local_succ, "SUCC %s %s %s\n", tokens[1], tokens[2], My_Node->succ_port);
                    n = write(fd, local_succ, strlen(local_succ));
                    if (n == -1)
                        exit(1);

                    sprintf(local_pred, "PRED %s\n", My_Node->node_id);
                    TCP_Client(My_Node->succ_ip, My_Node->succ_port, local_pred);

                    pred_fd = fd;
                    n = write(pred_fd, line, strlen(line));
                    if (n == -1)
                    exit(1);
                    pred_fd = fd;
                }
                else
                {
                    sprintf(local_succ, "SUCC %s %s %s\n", My_Node->succ_id, My_Node->succ_ip, My_Node->succ_port);
                    n = write(fd, local_succ, strlen(local_succ));
                    if (n == -1)
                        exit(1);
                    sprintf(local_entry, "ENTRY %s %s %s\n", tokens[1], tokens[2], tokens[3]);
                    n = write(pred_fd, local_entry, strlen(local_entry));
                    if (n == -1)
                        exit(1);
                    close(pred_fd);
                    pred_fd = fd;
                }
            }
        }
        else if (!strcmp(tokens[0], "SUCC"))
        {
            strcpy(My_Node->succsucc_id, tokens[1]);
            strcpy(My_Node->succsucc_ip, tokens[2]);
            strncpy(My_Node->succsucc_port, tokens[3], 5);
        }
        else if (!strcmp(tokens[0], "PRED"))
        {
            pred_fd = fd;
            strncpy(My_Node->pred_id, tokens[1], 2);
            
            sprintf(line, "ROUTE %s %s %s\n", My_Node->node_id, My_Node->node_id, My_Node->node_id);
            n = write(pred_fd, line, strlen(line));
            if (n == -1) {
                exit(1);
            }
            if (!broke_connection)
            {
                sprintf(local_succ, "SUCC %s %s %s\n", My_Node->succ_id, My_Node->succ_ip, My_Node->succ_port);
                n = write(pred_fd, local_succ, strlen(local_succ));
                if (n == -1)
                    exit(1);

                broke_connection = -1;
            }
        }
        else if (!strncmp(tokens[0], "ROUTE", 5))
        {
            ;
        }
    }
    return;
}

int Read_buffer_Nodeslist(char *buffer)
{
    char **lines = __ft_split(buffer, '\n');
    int new_id;
    char *id;

    srand(time(NULL));
    for (int i = 0; lines[i]; i++)
    {

        if (!strncmp(My_Node->node_id, lines[i], 2))
        {
            while(1)
            {
                new_id = (rand() % 99) + 1;

                sprintf(id, "%02d", new_id);
                for (int j = 0; lines[j]; j++)
                {
                    if (!strncmp(id, lines[j], 2))
                        break;
                    if(!lines[j + 1])
                        strcpy(My_Node->node_id, id);
                }
                if (!strncmp(id, My_Node->node_id, 2))
                    break;
            }
            return 1;
        }
    }
    return 0;
}

int Succ_from_Nodeslist(char *buffer)
{
    char **line_token;

    line_token = __ft_split (buffer, '\n');
    if (line_token[1])
    {
        char **tokens = __ft_split(line_token[1], ' ');
        My_Node->succ_id = tokens[0];
        My_Node->succ_ip = tokens[1];
        My_Node->succ_port = tokens[2]; 
        return 0;
    }
    return 1;
}

void noSucc(){
    strcpy(My_Node->succ_id, My_Node->node_id);
    strcpy(My_Node->succ_ip, My_Node->node_ip);
    strcpy(My_Node->succ_port, My_Node->node_port);

    strcpy(My_Node->succsucc_id, My_Node->node_id);
    strcpy(My_Node->succsucc_ip, My_Node->node_ip);
    strcpy(My_Node->succsucc_port, My_Node->node_port);

    strcpy(My_Node->pred_id, My_Node->node_id);
}

void newRoute(char *id, char *rpred, char *rsucc){
    Route *newRoute = malloc(sizeof(Route));

    strcpy(newRoute->id, id);
    strcpy(newRoute->route_pred, rpred);
    strcpy(newRoute->route_succ, rsucc);
    newRoute->next = Routes;
    Routes = newRoute;
}

void join()
{
    char buffer[200], local_reg[50], local_entry[50], local_nodes[10], local_route[55], nodes_buffer[200];
    int aux = 0;
    ssize_t n;

    strcpy(local_nodes, "NODES ");

    strcat(local_nodes, my_ring);

    UDP_Client(local_nodes, buffer);
    while (Read_buffer_Nodeslist(buffer) != 0)
    {   
        aux++;
    }
    if (aux != 0)
    {
        printf("Your id has been changed to %s", my_id);
    }

    TCP_Server(My_Node->node_port);
    sprintf(local_route, "ROUTE %s %s %s\n", My_Node->node_id,My_Node->node_id,My_Node->node_id);
    if (Succ_from_Nodeslist(buffer) != 0)
    {
        printf("--%s", local_route);
        noSucc();
    }
    sprintf(local_entry, "ENTRY %s %s %s\n", My_Node->node_id, My_Node->node_ip, My_Node->node_port);
    if (strcmp(My_Node->succ_id, My_Node->node_id) != 0)
    {

        TCP_Client(My_Node->succ_ip, My_Node->succ_port, local_entry);
    }
    sprintf(local_reg, "REG %s %s %s %s", my_ring, My_Node->node_id, My_Node->node_ip, My_Node->node_port);
    printf("%s\n", local_reg);
    UDP_Client(local_reg, nodes_buffer);
    if (strcmp(My_Node->succ_id, My_Node->node_id) != 0)
    {
        n = write(succ_fd, local_route, strlen(local_route));
        if (n == -1)
            exit(1);
    }

    return;
}

void leave()
{
    char local_unreg[13], unreg_buffer[8], local_entry[50];
    ssize_t n;

    strcpy(local_unreg, "UNREG ");
    strcat(local_unreg, my_ring);
    strcat(local_unreg, " ");
    strcat(local_unreg, my_id);
    strcat(local_unreg, "\0");
    UDP_Client(local_unreg, unreg_buffer);

    close(succ_fd);
    succ_fd = -1;
    close(pred_fd);
    pred_fd = -1;
    close(my_fd);
    my_fd = -1;

    memset(my_ring, 0, strlen(my_ring));
    memset(my_id, 0, strlen(my_id));
    memset(pred_id, 0, strlen(pred_id));
    memset(succ_id, 0, strlen(succ_id));
    memset(succsucc_id, 0, strlen(succsucc_id));
    memset(succ_ip, 0, strlen(succ_ip));
    memset(succsucc_ip, 0, strlen(succsucc_ip));
    memset(succ_port, 0, strlen(succ_port));
    memset(succsucc_port, 0, strlen(succsucc_port));

    return;
}
void Show_topology()
{
    printf("my id: %s\n", My_Node->node_id);
    printf("succ info: %s %s %s\n", My_Node->succ_id, My_Node->succ_ip, My_Node->succ_port);
    printf("succsucc info: %s %s %s\n", My_Node->succsucc_id, My_Node->succsucc_ip, My_Node->succsucc_port);
    printf("pred id: %s\n", My_Node->pred_id);
    return;
}

int Biggest_fd()
{
    int fd = -1;

    fd = my_fd;
    if (succ_fd > fd)
        fd = succ_fd;
    if (pred_fd > fd)

        fd = pred_fd;

    return fd;
}