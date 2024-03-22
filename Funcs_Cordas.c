#include "Funcs.h"

void UDP_Client(char *msg, char *buffer){
    int fd,errcode;
    ssize_t n;
    socklen_t addrlen;
    struct addrinfo hints, *res;
    struct sockaddr_in addr;

    fd=socket(AF_INET,SOCK_DGRAM,0);
    if(fd==-1) exit(1);

    memset(&hints,0,sizeof hints);
    hints.ai_family=AF_INET;
    hints.ai_socktype=SOCK_DGRAM;

    errcode=getaddrinfo("tejo.tecnico.ulisboa.pt","59000",&hints,&res);
    if(errcode!=0) exit(1);

    n=sendto(fd,msg,strlen(msg),0,res->ai_addr,res->ai_addrlen);
    if(n==-1)  exit(1);

    addrlen=sizeof(addr);
    n=recvfrom(fd,buffer,200,0,(struct sockaddr*)&addr,&addrlen);
    if(n==-1) exit(1);

    write(1,"echo: ",6); write(1,buffer,n);

    freeaddrinfo(res);
    close(fd);

    return;
}

void TCP_Client(char *ip, char *port, char *msg){
    int fd,errcode;
    ssize_t n;
    struct addrinfo hints, *res;
    
    fd=socket(AF_INET,SOCK_STREAM,0);
    if(fd==-1) exit(1);

    succ_fd = fd;

    memset(&hints,0,sizeof hints);
    hints.ai_family=AF_INET;
    hints.ai_socktype=SOCK_STREAM;

    errcode=getaddrinfo(ip,port,&hints,&res);
    if(errcode!=0) exit(1);


    n=connect(fd,res->ai_addr,res->ai_addrlen);
    if(n==-1) exit(1);

    n=write(fd,msg,strlen(msg));
    if(n==-1) exit(1);

    freeaddrinfo(res);
    
    return;
}

void TCP_Server(char *port){
    int fd,errcode;
    ssize_t n;
    struct addrinfo hints, *res;
    
    fd=socket(AF_INET,SOCK_STREAM,0);
    if(fd==-1) exit(1);

    my_fd = fd;

    memset(&hints,0,sizeof hints);
    hints.ai_family=AF_INET;
    hints.ai_socktype=SOCK_STREAM;
    hints.ai_flags=AI_PASSIVE;

    errcode=getaddrinfo(NULL,port,&hints,&res);
    if(errcode!=0) exit(1);

    n=bind(fd,res->ai_addr,res->ai_addrlen);
    if(n==-1) exit(1);

    if(listen(fd,5)==-1) exit(1);

    freeaddrinfo(res);

    return;
}

void Read_buffer_space_udp(char *buffer, char *id, char *ip, char *port){
    char *token;
    int aux=0;

    token = strtok(buffer," ");
    do
   {
        if(aux==0) aux++;
        else if(aux==1)
        {
            strcpy(id,token);
            aux++;
        }
         else if(aux==2)
        {
            strcpy(ip,token);
            aux++;
        }
         else if(aux==3)
        {
            strcpy(port,token);
            aux++;
        }
        
   }
   while ((token = strtok(NULL, " ")));
    return;
}

void Manutencao_array_chords(int pos){
    int i;

    for (i = pos; i < last_chord_fd; i++) {
        chords_fds[i] = chords_fds[i+1];
        strcpy(chords_ids[i],chords_ids[i+1]);
    }
    chords_fds[last_chord_fd] = -1;
    memset(chords_ids[last_chord_fd], '\0', sizeof(chords_ids[last_chord_fd]));
    last_chord_fd = (last_chord_fd - 1);
    return;
}

int Recieve_chord(char *id, int fd){

    if(last_chord_fd == 12){
        printf("Too many chords");
        return 1;
    }

    last_chord_fd++;
    chords_fds[last_chord_fd] = fd;
    strcpy(chords_ids[last_chord_fd],id);
    
    return 0;
}

void Remove_chord(){
    close(my_chord_fd);
    my_chord_fd = -1;
    memset(my_chord_id,'\0',sizeof(my_chord_id));
    return;
}

void Read_buffer_tcp(int fd){
    char *token, line[50], local_succ[50], local_entry[50], local_pred[50], local_chat[50], buffer[200], id[3], ip[20], port[20], from[3], to[3], message[50];
    int aux=0,i;
    ssize_t n;

    n=read(fd,buffer,200);
    if(n==-1) exit(1);

    if(n == 0){
        if(fd == succ_fd){
            close(succ_fd);
            succ_fd = -1;
            memset(succ_id,'\0',sizeof(succ_id));
            memset(succ_ip,'\0',sizeof(succ_ip));
            memset(succ_port,'\0',sizeof(succ_port));
            strcpy(succ_id, succsucc_id);
            strcpy(succ_ip,succsucc_ip);
            strcpy(succ_port,succsucc_port);

            strcpy(local_succ,"SUCC ");
            strcat(local_succ, succ_id); strcat(local_succ," "); strcat(local_succ,succ_ip); strcat(local_succ," "); strcat(local_succ,succ_port); strcat(local_succ,"\n");strcat(local_succ,"\0");
            n=write(pred_fd,local_succ,strlen(local_succ));
            if(n==-1) exit(1);

            strcpy(local_pred,"PRED ");
            strcat(local_pred, my_id); strcat(local_pred,"\n"); strcat(local_pred,"\0");
            TCP_Client(succ_ip,succ_port,local_pred);
        }
        else if(fd == pred_fd){
            close(pred_fd);
            pred_fd = -1;
            broke_connection = 0;
        }
        else if(fd == my_chord_fd){
            Remove_chord();
        }
        else{
            for(i=0; i<=last_chord_fd; i++){
                if(fd == chords_fds[i]){
                    Manutencao_array_chords(i);
                }
                close(fd);
            }
        }

    }
    else{ 
    write(1,"received: ",10);write(1,buffer,n);

    token = strtok(buffer,"\n");
    strcpy(line,token);

    token = strtok(line," ");
    if(strcmp(token,"ENTRY")==0){
        if(fd == succ_fd){
            strcpy(succsucc_id,succ_id);
            strcpy(succsucc_ip,succ_ip);
            strcpy(succsucc_port,succ_port);
            memset(succ_id,'\0',sizeof(succ_id));
            memset(succ_ip,'\0',sizeof(succ_ip));
            memset(succ_port,'\0',sizeof(succ_port));
            close(succ_fd);
            succ_fd = -1;
            aux=0;
        while ((token = strtok(NULL, " "))){
            if(aux==0){
                strcpy(id,token);
                strcpy(succ_id,token);
                aux++;
            }else if(aux==1){
                strcpy(ip,token);
                strcpy(succ_ip,token);
                aux++;
            }else if(aux==2){
                strcpy(port,token);
                strcpy(succ_port,token);
                aux++;
            }
        }

        strcpy(local_succ,"SUCC ");
        strcat(local_succ, id); strcat(local_succ," "); strcat(local_succ,ip); strcat(local_succ," "); strcat(local_succ,port); strcat(local_succ,"\n");strcat(local_succ,"\0");
        n=write(pred_fd,local_succ,strlen(local_succ));
        if(n==-1) exit(1);

        strcpy(local_pred,"PRED ");
        strcat(local_pred, my_id); strcat(local_pred,"\n"); strcat(local_pred,"\0");
        TCP_Client(succ_ip,succ_port,local_pred);
        }
        else{
            memset(pred_id,'\0',sizeof(pred_id));
            aux=0;
            while ((token = strtok(NULL, " "))){
            if(aux==0){
            strcpy(id,token);
            strcpy(pred_id,token);
            aux++;
            }else if(aux==1){
            strcpy(ip,token);
            aux++;
            }else if(aux==2){
            strcpy(port,token);
                aux++;
            }
            }

            if(pred_fd == -1){
                strcpy(succ_id,id);
                strcpy(succ_ip,ip);
                strcpy(succ_port,port);

                strcpy(local_succ,"SUCC ");
                strcat(local_succ, succ_id); strcat(local_succ," "); strcat(local_succ,succ_ip); strcat(local_succ," "); strcat(local_succ,succ_port); strcat(local_succ,"\n");strcat(local_succ,"\0");
                n=write(fd,local_succ,strlen(local_succ));
                if(n==-1) exit(1);

                strcpy(local_pred,"PRED ");
                strcat(local_pred, my_id); strcat(local_pred,"\n");strcat(local_pred,"\0");
                TCP_Client(ip,port,local_pred);
                pred_fd = fd;
            }
            else{
                strcpy(local_succ,"SUCC ");
                strcat(local_succ, succ_id); strcat(local_succ," "); strcat(local_succ,succ_ip); strcat(local_succ," "); strcat(local_succ,succ_port); strcat(local_succ,"\n");strcat(local_succ,"\0");
                n=write(fd,local_succ,strlen(local_succ));
                if(n==-1) exit(1);

                strcpy(local_entry,"ENTRY ");
                strcat(local_entry, id); strcat(local_entry," "); strcat(local_entry,ip); strcat(local_entry," "); strcat(local_entry,port); strcat(local_entry,"\n");strcat(local_entry,"\0");
                n=write(pred_fd,local_entry,strlen(local_entry));
                if(n==-1) exit(1);
                close(pred_fd);
                pred_fd = -1;
                pred_fd = fd;
            }
            }
            
        
    }else if(strcmp(token,"SUCC")==0){
        memset(succsucc_id,'\0',sizeof(succsucc_id));
        memset(succsucc_ip,'\0',sizeof(succsucc_ip));
        memset(succsucc_port,'\0',sizeof(succsucc_port));
        aux=0;
        while ((token = strtok(NULL, " "))){
            if(aux==0){
                strcpy(succsucc_id,token);
                aux++;
            }else if(aux==1){
                strcpy(succsucc_ip,token);
                aux++;
            }else if(aux==2){
                strcpy(succsucc_port,token);
                aux++;
            }
        }
    }else if(strcmp(token,"PRED")==0){
        memset(pred_id,'\0',sizeof(pred_id));
        pred_fd = fd;

        token = strtok(NULL, "\n");
        strcpy(pred_id,token);

        if(broke_connection == 0){
            strcpy(local_succ,"SUCC ");
            strcat(local_succ, succ_id); strcat(local_succ," "); strcat(local_succ,succ_ip); strcat(local_succ," "); strcat(local_succ,succ_port); strcat(local_succ,"\n");strcat(local_succ,"\0");
            n=write(pred_fd,local_succ,strlen(local_succ));
            if(n==-1) exit(1);

            broke_connection = -1;
        }
    }else if(strcmp(token,"CHAT")==0){
        token = strtok(NULL," ");
        strcpy(from,token);
        token = strtok(NULL," ");
        strcpy(to,token);
        token = strtok(NULL," ");
        strcpy(message,token);

        if(strcmp(from,my_id)==0){
            printf("Destination does not exist");
            return;
        }

        if(strcmp(my_id,to) == 0){
            printf("Message from %s: %s",from,message);
        }
        else{
            strcpy(local_chat, "CHAT ");
            strcat(local_chat,from);
            strcat(local_chat," ");
            strcat(local_chat,to);
            strcat(local_chat," ");
            strcat(local_chat,message);
            strcat(local_chat,"\n");

            n=write(succ_fd,local_chat,strlen(local_chat));
            if(n==-1)
            exit(1);
        }

    }else if(strcmp(token,"CHORD")==0){
        token = strtok(NULL, "\n");
        if(Recieve_chord(token,fd) != 0){
            close(fd);
        }
    }
    }
    
    return;
}

int Chord(){
    char *line_token, buffer[200], id[3],ip[20],port[20],local_nodes[10],local_chord[20];
    int aux = 0;
    int fd,errcode;
    ssize_t n;
    struct addrinfo hints, *res;

    strcpy(local_nodes, "NODES ");
    strcat(local_nodes, my_ring);
    UDP_Client(local_nodes, buffer);

    line_token = strtok(buffer, "\n");
    do{
        if (aux == 0)
        {
            aux++;
        }
        else
        {
            sscanf(line_token, "%s %s %s", id, ip, port);
            
            if (strcmp(succ_id, id) != 0 && strcmp(pred_id, id) != 0 && strcmp(my_id, id) != 0)
            {
                

                strcpy(local_chord, "CHORD "); strcat(local_chord,my_id); strcat(local_chord, "\n"); strcat(local_chord, "\0");
            
                fd=socket(AF_INET,SOCK_STREAM,0);
                if(fd==-1) exit(1);

                my_chord_fd = fd;
                strcpy(my_chord_id,id);

                memset(&hints,0,sizeof hints);
                hints.ai_family=AF_INET;
                hints.ai_socktype=SOCK_STREAM;

                errcode=getaddrinfo(ip,port,&hints,&res);
                if(errcode!=0) exit(1);


                n=connect(fd,res->ai_addr,res->ai_addrlen);
                if(n==-1) exit(1);

                n=write(fd,local_chord,strlen(local_chord));
                if(n==-1) exit(1);

                freeaddrinfo(res);
                return 0;
            }
        }
    }
    while ((line_token = strtok(NULL, "\n")));
    return 1;
}

void Read_buffer_LF(char *buffer, char *id, char *ip, char *port){
    char *token,line[50];

    token = strtok(buffer,"\n");
    do
   {
      strcpy(line,token);
      Read_buffer_space_udp(line, id, ip, port);
   }
   while ((token = strtok(NULL, "\n")));
    return;
}

int Read_buffer_Nodeslist(char *buffer){
    char *line_token, *id_token;
    int aux=0,l=0,c=0;
    char id_list[11]="0123456789";

    line_token = strtok(buffer,"\n");
    do
   {
    if(aux==0){
        aux++;
    }
    else{
      
     id_token = strtok(line_token," ");
      if(strcmp(my_id,id_token)==0){
        my_id[1]=id_list[l];
            l++;
            if(l==9){
                l=0;
                c++;
            }
            my_id[0]=id_list[c];
        return 1;
      }
      }
   }
   while ((line_token = strtok(NULL, "\n")));
    return 0;
}

int Succ_from_Nodeslist(char *buffer){
    char *line_token, *id_token, line[50];
    int aux_read=0;

    line_token = strtok(buffer,"\n");
   if((line_token = strtok(NULL, "\n")) != NULL){
     strcpy(line,line_token);
     id_token = strtok(line, " ");
     strcpy(succ_id,id_token);
    while((id_token = strtok(NULL, " "))){
        if(aux_read==0)
        {
            strcpy(succ_ip,id_token);
            aux_read++;
        }
         else if(aux_read==1)
        {
            strcpy(succ_port,id_token);
            aux_read++;
            return 0;
        }

    }
    return 0;
   }
   
   return 1;
}

void join(){
    char buffer[200], local_reg[50], local_entry[50], local_nodes[10],nodes_buffer[200],aux_buffer[200];
    int aux=0;

    strcpy(local_reg,"REG "); strcpy(local_entry,"ENTRY "); strcpy(local_nodes,"NODES ");

    strcat(local_nodes, my_ring);
    UDP_Client(local_nodes,buffer);

    strcpy(aux_buffer,buffer);

     while(Read_buffer_Nodeslist(aux_buffer) != 0){
        aux++;
     }

    if(aux != 0){
        printf("Your id has been changed to %s", my_id);
    }

    TCP_Server(my_port);

    strcpy(aux_buffer,buffer);
    if(Succ_from_Nodeslist(aux_buffer) != 0){
        strcpy(succ_id,my_id);
        strcpy(succ_ip,my_ip);
        strcpy(succ_port,my_port);
        strcpy(pred_id,my_id);
        strcpy(succsucc_id,my_id);
        strcpy(succsucc_ip,my_ip);
        strcpy(succsucc_port,my_port);
    }

    strcat(local_entry, my_id); strcat(local_entry," "); strcat(local_entry,my_ip); strcat(local_entry," "); strcat(local_entry,my_port); strcat(local_entry,"\n");strcat(local_entry,"\0");
    if (strcmp(succ_id,my_id) != 0){
        TCP_Client(succ_ip,succ_port,local_entry);
    } 

    strcat(local_reg, my_ring); strcat(local_reg," "); strcat(local_reg,my_id); strcat(local_reg," "); strcat(local_reg,my_ip); strcat(local_reg," "); strcat(local_reg,my_port);strcat(local_reg,"\0");
    //printf("reg  is %ld %s|\n",strlen(local_reg), local_reg);
    UDP_Client(local_reg,nodes_buffer);

    return;
}

void leave(){
    char local_unreg[13], unreg_buffer[8];

    strcpy(local_unreg,"UNREG ");
    strcat(local_unreg, my_ring); strcat(local_unreg," "); strcat(local_unreg,my_id);strcat(local_unreg,"\0");
    UDP_Client(local_unreg,unreg_buffer);

    close(succ_fd);
    succ_fd = -1;
    close(pred_fd);
    pred_fd = -1;
    close(my_fd);
    my_fd = -1;

    memset(my_ring,'\0',sizeof(my_ring));
    memset(my_id,'\0',sizeof(my_id));
    memset(pred_id,'\0',sizeof(pred_id));
    memset(succ_id,'\0',sizeof(succ_id));
    memset(succsucc_id,'\0',sizeof(succsucc_id));
    memset(succ_ip,'\0',sizeof(succ_ip));
    memset(succsucc_ip,'\0',sizeof(succsucc_ip));
    memset(succ_port,'\0',sizeof(succ_port));
    memset(succsucc_port,'\0',sizeof(succsucc_port));

    return;
}

void Show_topology(){
    printf("my id: %s\n",my_id);
    printf("succ info: %s %s %s\n",succ_id,succ_ip,succ_port);
    printf("succsucc info: %s %s %s\n",succsucc_id,succsucc_ip,succsucc_port);
    printf("pred id: %s\n",pred_id);
    if(my_chord_fd != -1){
        printf("my chord id: %s\n",my_chord_id);
    }
    for(int i=0; i<=last_chord_fd; i++){
        printf("chord %d id: %s\n",(i+1),chords_ids[i]);
    }
    return;
}

int Biggest_fd(){
    int fd=-1,i;

    fd = my_fd;

    if(succ_fd > fd) fd = succ_fd;

    if(pred_fd > fd) fd = pred_fd;

    if(my_chord_fd > fd) fd = my_chord_fd;

    for(i=0; i<=last_chord_fd; i++){
        if(chords_fds[i] > fd) fd = chords_fds[i];
    }

    return fd;
}

