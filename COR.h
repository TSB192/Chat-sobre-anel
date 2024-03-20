#ifndef COR_H
#define COR_H

#define max(A,B) ((A)>=(B)?(A):(B))

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

#define PORT "59000"
#define MAX_NODES 16 // Maximum number of nodes in the list

// typedef struct for nodes
typedef struct Node
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

}Node;



#endif