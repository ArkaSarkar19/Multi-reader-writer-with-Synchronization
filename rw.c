#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/un.h>
#include <pthread.h>
#include <sys/shm.h>
#include <signal.h>
#define server_path  "server_socket"

void error(char *message);
void listen1(int client_socket);
void handle_reader(int client_socket);
void handle_writer(int client_socket);
void cleanup();
pthread_t thread;
pthread_t x;



void error(char *message){
    perror(message);
    exit(0);
}

void listen1(int client_socket){
    while(1){
        char buffer[256] = {};
        int i;
        if(i= recv(client_socket,buffer,256,0)==-1){
            error("Error in revieving");
        }
    //    buffer[i] = '\0';
     printf("NEW MESSAGE : %s\n", buffer);

    }
}
void cleanup(int sig){
        fflush(stdout);

    printf("BYE !!!! \n");

    pthread_join(thread,NULL);
    exit(0);
}


void handle_reader(int client_socket){
    while(1){
        char buffer[256];
        printf("Select Operation to be performed \n");
        printf("1 - read data \n");
        printf("2 - dequeue data \n");
        fgets(buffer,256,stdin);
        if(atoi(buffer) == 1)
        {
            int s_status = send(client_socket,buffer,256,0);
            if(s_status == -1)
            {
                printf("Can't send data to server by reader \n");
            }
            char data1[256];
            printf("Enter index : ");
            fgets(data1,256,stdin);
            int s2_status = send(client_socket,data1,256,0);
            if(s2_status == -1)
            {
                printf("Can't send data2 to server by reader \n");
            }

        }
        else if(atoi(buffer) == 2)
        {
            int s_status = send(client_socket,buffer,256,0);
            if(s_status == -1)
            {
                printf("Can't send data to server by reader \n");
            }
            printf("Reading data ............. \n");

        }
        else ;

    }
}

void handle_writer(int client_socket){
    while(1){
        char buffer[256];
        printf("Select Operation to be performed \n");
        printf("1 - overwrite data \n");
        printf("2 - enqueue data \n");
        fgets(buffer,256,stdin);
        if(atoi(buffer) == 1)
        {
            int s_status = send(client_socket,buffer,256,0);
            if(s_status == -1)
            {
                printf("Can't send data to server by writer \n");
            }
            char data1[256];
            printf("Enter index : ");
            fgets(data1,256,stdin);
            int s2_status = send(client_socket,data1,256,0);
            if(s2_status == -1)
            {
                printf("Can't send data1 to server by writer \n");
            }
            char data2[256];
            printf("Enter value : ");
            fgets(data2,256,stdin);
            int s3_status = send(client_socket,data2,256,0);
            if(s3_status == -1)
            {
                printf("Can't send data2 to server by writer \n");
            }

        }
        if(atoi(buffer) == 2)
        {
            int s_status = send(client_socket,buffer,256,0);
            if(s_status == -1)
            {
                printf("Can't send data to server by writer \n");
            }
            char data2[256];
            printf("Enter value to enqueue: ");
            fgets(data2,256,stdin);
            int s3_status = send(client_socket,data2,256,0);
            if(s3_status == -1)
            {
                printf("Can't send data2 to server by writer \n");
            }
        }
    }
}

int main(){
    int client_socket;  
    client_socket =  socket(AF_UNIX, SOCK_STREAM,0);
    printf("%d\n",client_socket);
      if(client_socket == -1){
        error("Error in opening socket \n");
    }
    
    struct sockaddr_un server_address;

    server_address.sun_family = AF_UNIX;
    sprintf(server_address.sun_path, "%s", server_path);

    int client_connection = connect(client_socket,(struct sockaddr *)&server_address, sizeof(server_address));
    
    if(client_connection==-1){
        error("Cannot connect to server \n");
    }

    // if(send(client_socket,client_name,strlen(client_name),0)==-1) error("Error server didn't respond");
    printf("Connected to Server \n");
    pthread_create(&thread,NULL,listen1,(void *)client_socket);
    char buffer[256];
    printf("what do you want to be ? WRITER or READER : ");
    fgets(buffer,256,stdin);

    if(strcmp(buffer,"READER") == 10)
    {
        int s_status = send(client_socket, buffer,strlen(buffer),0);
        if(s_status == -1)
        {
            printf("Error sending from reader socket \n");
        }
        //pthread_create(&x,NULL,handle_reader,(void *)client_socket);
        //printf("jdbidbibvbivbidv");
            while(1){
        char buffer[256];
        printf("Select Operation to be performed \n");
        printf("1 - read data \n");
        printf("2 - dequeue data \n");
        fgets(buffer,256,stdin);
        if(atoi(buffer) == 1)
        {
            int s_status = send(client_socket,buffer,256,0);
            if(s_status == -1)
            {
                printf("Can't send data to server by reader \n");
            }
            char data1[256];
            printf("Enter index : ");
            fgets(data1,256,stdin);
            int s2_status = send(client_socket,data1,256,0);
            if(s2_status == -1)
            {
                printf("Can't send data2 to server by reader \n");
            }

        }
        else if(atoi(buffer) == 2)
        {
            int s_status = send(client_socket,buffer,256,0);
            if(s_status == -1)
            {
                printf("Can't send data to server by reader \n");
            }
            printf("Reading data ............. \n");

        }
        else {
            printf("Enter either 1 or 2");
        }

    }

    }
    else if(strcmp(buffer,"WRITER") == 10)
    {
        int s_status = send(client_socket, buffer,strlen(buffer),0);
        if(s_status == -1)
        {
            printf("Error sending from writer socket \n");
        }
        //pthread_create(&x,NULL,handle_writer,(void *)client_socket);
            while(1){
        char buffer[256];
        printf("Select Operation to be performed \n");
        printf("1 - overwrite data \n");
        printf("2 - enqueue data \n");
        fgets(buffer,256,stdin);
        if(atoi(buffer) == 1)
        {
            int s_status = send(client_socket,buffer,256,0);
            if(s_status == -1)
            {
                printf("Can't send data to server by writer \n");
            }
            char data1[256];
            printf("Enter index : ");
            fgets(data1,256,stdin);
            int s2_status = send(client_socket,data1,256,0);
            if(s2_status == -1)
            {
                printf("Can't send data1 to server by writer \n");
            }
            char data2[256];
            printf("Enter value : ");
            fgets(data2,256,stdin);
            int s3_status = send(client_socket,data2,256,0);
            if(s3_status == -1)
            {
                printf("Can't send data2 to server by writer \n");
            }

        }
        else if(atoi(buffer) == 2)
        {
            int s_status = send(client_socket,buffer,256,0);
            if(s_status == -1)
            {
                printf("Can't send data to server by writer \n");
            }
            char data2[256];
            printf("Enter value to enqueue: ");
            fgets(data2,256,stdin);
            int s3_status = send(client_socket,data2,256,0);
            if(s3_status == -1)
            {
                printf("Can't send data2 to server by writer \n");
            }
        }
        else {
            printf("Enter either i or 2");
        }
    }

    }
    else;
    close(client_socket);
    return 0;
}
