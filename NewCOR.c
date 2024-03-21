#include "FUNCS.h"
#define NPORT "59000"

char my_ring[4], my_id[3], my_ip[16], my_port[6], succ_id[3], succ_ip[16], succ_port[6], succsucc_id[3], succsucc_ip[16], succsucc_port[6], pred_id[3], index_linha[16][3], index_coluna[16][3], encaminhamento[16][16][50], caminhos[16][50], expedicao[16][2];
int succ_fd = -1, pred_fd = -1, my_fd = -1, broke_connection = -1, index_coluna_max = -1, index_linha_max = -1, index_cost[16];

int main(int argc, char *argv[])
{
    // My_node is where we will save the Node information -- allocated 1 slots of memory
    Node **My_Node = calloc(1, sizeof(Node *));
    
    char *token, buffer[200], ring[4], id[3], command[3];
    int fd, counter, i, j;
    fd_set rfds;
    int newfd;
    socklen_t addrlen;
    struct sockaddr_in addr;

    strcpy(my_ip, argv[1]);
    strcpy(my_port, argv[2]);

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

    printf("Write j ring id to register your node:\n");
    scanf("%s %s %s", command, ring, id);
    strcpy(my_id, id);
    strcpy(my_ring, ring);

    join();

    while (1)
    {
        fd = Biggest_fd();
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
                join();
            }
            else if (buffer[0] == 'l')
            {

                leave();
            }
            else if (buffer[0] == 's')
            {
                if (buffer[1] == 't')
                {
                    Show_topology();
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
                        Show_topology();
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
}