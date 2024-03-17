#ifndef COR_H
#define COR_H

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <time.h>


#define PORT "59000"
#define MAX_NODES 10 // Maximum number of nodes in the list

// variáveis globais
extern char my_id[3], my_ip[16], my_port[6], succ_id[3], succ_ip[16], succ_port[6], succsucc_id[3], succsucc_ip[16], succsucc_port[6], pred_id[3];
extern int succ_fd, pred_fd, my_fd;


// funções de criação de client UDP
void UDP_client(char *msg);

// funções de criação de client/server TCP
void TCP_Client(char *ip, char *port, char *msg);
void TCP_Server(char *port);

// funções de leitura de buffer
void Read_buffer_space_udp(char *buffer, char *id, char *ip, char *port);
void Read_buffer_LF(char *buffer, char *id, char *ip, char *port);
int Read_buffer_Nodeslist(char *buffer);
int Succ_from_Nodeslist(char *buffer);
void Read_buffer_tcp(int fd);



#endif