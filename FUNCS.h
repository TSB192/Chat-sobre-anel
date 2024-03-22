#ifndef FUNCS_H
#define FUNCS_H

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>

// typedef struct for nodes
typedef struct Node
{
    char *node_id; 
    char *succ_id;
    char *succsucc_id;   
    char *chord_id;  
    char *pred_id; 

    char *node_ip; 
    char *succ_ip;   
    char *succsucc_ip;   
    char *chord_ip;

    char *node_port;
    char *succ_port;   
    char *succsucc_port;
    char *chord_port;  
}Node;


typedef struct Table
{
    char *node_id; 
    char *node_ip; 
    char *node_port;
    char *succ_id;
    char *succ_ip;   
    char *succ_port;   
    char *succsucc_id;   
    char *succsucc_ip;   
    char *succsucc_port;
    char *pred_id; 
    char *chord_id;  
    char *chord_ip;  
    char *chord_port;  
}Table;
//variáveis globais
extern char my_ring[4],my_id[3],my_ip[16],my_port[6],succ_id[3],succ_ip[16],succ_port[6], succsucc_id[3],succsucc_ip[16],succsucc_port[6],pred_id[3],index_linha[16][3],index_coluna[16][3],encaminhamento[16][16][50],caminhos[16][50],expedicao[16][2];
extern int succ_fd,pred_fd,my_fd,broke_connection,index_coluna_max,index_linha_max,index_cost[16];
extern Node *My_Node;


int Biggest_fd();
//funções de criação de client UDP
void UDP_Client(char *msg, char *buffer);
//funções de criação de client/server TCP
void TCP_Client(char *ip, char *port, char *msg);
void TCP_Server(char *port);
//funções de leitura de buffer
int Read_buffer_Nodeslist(char *buffer);
int Succ_from_Nodeslist(char *buffer);
void Read_buffer_tcp(int fd);
//funções de interface de utilizador
//int Read_input();
void join();
void leave();
void Show_topology();
int Biggest_fd();
void Create_Node();

#endif