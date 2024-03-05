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
#include <stdlib.h>
#include <time.h>
#define PORT "58001"
#define MAX_NODES 10 // Maximum number of nodes in the list

typedef struct Node
{
    int id;
    int TCP;
    int pred;
    int succ;
    int succsucc;
}Node;


typedef struct Ring
{
    Node *nodelist;
}Ring;


#endif