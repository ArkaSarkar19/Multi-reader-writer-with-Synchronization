#include<stdio.h>
#include<semaphore.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/un.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <pthread.h>
#include <signal.h>
#define path "server_socket"

#define MAX 1000
#define MAX_rw 100
char queue[MAX];
int front = 0;
int rear = 1 ;
int curr_readers = 0;
int curr_writers = 0;
sem_t deq_mutex;
sem_t enq_mutex;
sem_t write_mutex;
sem_t r1_mutex[MAX];
sem_t r2_mutex[MAX];
int r_num[MAX];

void enqueue(int element);
void error(char *messgae);
void f_reader(void * args);
void f_writer(void * args);
void display_queue();
int dequeue();

struct  reader
{
  int number;
	struct sockaddr_un reader_address;

};

struct writer
{
  int number;
  struct sockaddr_un writer_address;

};

pthread_t r_pthread[MAX_rw];
pthread_t w_pthread[MAX_rw];
struct reader all_readers[MAX_rw];
struct writer all_writers[MAX_rw];

void error(char *message){
    perror(message);
    exit(0);
}

void f_reader(void * args)
{
  struct reader *curr_reader = args;
  while(1)
  {
    display_queue();
    char o[256];
    int r_status = recv(curr_reader->number,o,256,0);
    if(r_status == -1)
    {
      printf("Cannot recieve operation from reader %d.\n", curr_reader->number);
    }
    //o == 1 when reader wants to read data of queue
    if(atoi(o) == 1) 
    {
      char index[256];
      int i_status = recv(curr_reader->number,index,256,0);
      int i = atoi(index);
      printf("reader index %d\n", i);
      sem_wait(&r1_mutex[i]);
      int done = 1;
      if(r_num[i] == 0)
	      {
		     sem_wait(&r2_mutex[i]);
		      if(i < front || i >= rear)
		        {
			        printf("Index %d is out of bounds for reader %d.\n", i, curr_reader->number);
              char msg[256] = "Index out of bounds \n";
              int s_status = send(curr_reader->number,msg,strlen(msg),0);
              if(s_status == -1)
              {
                printf("Error1 sending back to  reader %d\n", curr_reader->number);
              }
            sem_post(&r2_mutex[i]);

              sem_post(&r1_mutex[i]);
              done = 0;
          }

	    }
      if(done)
      {
        r_num[i]++;
        sem_post(&r1_mutex[i]);

        // int value = queue[i];
        char value[256];
        sprintf(value,"%d",queue[i]);
        printf(" Element read from index %d is %s by reader %d.\n", i, value, curr_reader->number);
        int s_status = send(curr_reader->number,value,256,0);
        if(s_status == -1 )
          {
            printf("Error2 sending back to reader %d \n", curr_reader->number);

          }
        sem_wait(&r1_mutex[1]);
        r_num[i]--;
        if(r_num[i] == 0)
        {
          sem_post(&r2_mutex[i]);
        }
        sem_post(&r1_mutex[i]);
      }     
      

    }

    //o == 2 reader wants to dequeue a data of the queue
    if(atoi(o) == 2)
    {
	    sem_wait(&deq_mutex);
        if(front == rear)
        {
          char msg[256] = "Cannot dequeue \n";
          int s_status = send(curr_reader->number,msg,strlen(msg),0);
          if(s_status == -1)
          {
            printf("Error3 sending back to reader %d", curr_reader->number);
          }
        }
        else
        {
          int pos = front;
          // sem_wait(&r2_mutex[pos]);
          front++;
        int value = queue[front];
                char value[256];
      sprintf(value,"%d",queue[front]);
          printf("The value of the element dequeued by reader %d from index %d is %s .\n", curr_reader->number, pos, value);
          int s_status = send(curr_reader->number,value,256,0);
          if(s_status == -1)
          {
            printf("Error4 sending back to reader %d\n", curr_reader->number);
          }
          // sem_post(&r2_mutex[pos]);
        }
        sem_post(&deq_mutex);
    }


  }
}

void f_writer(void * args){
  struct writer *curr_writer = args;
  while(1)
  {
        display_queue();

    char o[256];
    int r_status = recv(curr_writer->number,o,256,0);
    if(r_status == -1)
    {
      printf("Cannot recieve operation from writer %d.\n", curr_writer->number);
    }
    //o == 1 writer wants to overwrite a data in the queue
    printf("%d\n",atoi(o));
    if(atoi(o) == 1)
    {
      char index[256];
      int s_status = recv(curr_writer->number,index,256,0);
      if(s_status == -1)
      {
        printf("Error recieving op from writer %d\n", curr_writer->number);
      }
      int i = atoi(index);
      sem_wait(&write_mutex);
      sem_wait(&r2_mutex[i]);
      int done = 1;
      if(i < front || i >= rear)
        {
          char msg[256] = "Index out of bounds \n";
          int s_status = send(curr_writer->number,msg,256,0);
          if(s_status == -1)
          {
            printf("Error1 sending back to writer %d", curr_writer->number);
          }
          sem_post(&r2_mutex[i]);
          sem_post(&write_mutex);
          done = 0;
        }
        if(done)
        {
        char value[256];
        int v_status = recv(curr_writer->number,value,256,0);
        if(v_status == -1)
        {
        printf("Error in recieving value from writer %d \n", curr_writer->number);
        }
        queue[i] = atoi(value);
        char msg[256];
        strcat(msg, "Value written to queue is ");
        sprintf(msg," %d",atoi(value));
        printf("Value %d  written  at index %d by writer \n",queue[i],i, curr_writer->number, queue[i]);
        sem_post(&r2_mutex[i]);
        sem_post(&write_mutex);
        }

    }

    //o == 2 writer wants to enqueue a element in queue
    if(atoi(o) == 2)
    {
      sem_wait(&write_mutex);
	    sem_wait(&enq_mutex);
	    if(rear == MAX)
	      {
	      	printf("Queue full.\n");
          char msg[256] = "Cannot Enqueue Queue full \n";

	      }
	    else
	      {
          char value[256];
          int v_status = recv(curr_writer->number,value,256,0);
          if(v_status == -1)
          {
            printf("Error recieving value from writer %d \n",curr_writer->number);
          }
          int i = rear;
          sem_wait(&r2_mutex[i]);
          queue[rear] = atoi(value);
          rear++;
          printf("%d\n", rear);
          printf("%s\n", value);

          printf("Element %d enqueued\n", queue[i]);
          char msg[256] = "Enqueued successfully";
          int s_status = send(curr_writer->number,msg,256,0);
          
          if(s_status == -1)
          {
            printf("Error2 sending back to writer %d", curr_writer->number);
          }
          sem_wait(&r2_mutex[i]);
          
        }
      sem_post(&enq_mutex);
      sem_post(&write_mutex);
    }
  }

}

void display_queue(){
  printf("\n");
  printf("current queue ");
  for(int i=front;i<rear;i++){
    printf(", %d",queue[i]);
  }
    printf("\n");

}


int main(){

    int i;
    for(i = 0; i < MAX; i++)
      {
        r_num[i] = 0;
        sem_init(&r1_mutex[i], 0, 1);
        sem_init(&r2_mutex[i], 0, 1);
      }
    sem_init(&write_mutex, 0, 1);
    sem_init(&deq_mutex, 0, 1);
    sem_init(&enq_mutex, 0, 1);
    int server_socket,client_socket;
    server_socket = socket(AF_UNIX, SOCK_STREAM,0);

    if(server_socket == -1)
    {
        printf("Error in opening socket \n");
    }
    printf("%d\n",server_socket);
    struct sockaddr_un address_server,address_client;
    address_server.sun_family = AF_UNIX;
    strcpy(address_server.sun_path,path);
    unlink(address_server.sun_path);
    int server_bind = bind(server_socket,(struct sockaddr *) &address_server,  sizeof(address_server));
    if(server_bind == -1)
    {
        printf("Error establishing a server socket connection \n");
    }
    if(listen(server_socket,100) == -1)
  {
        printf("Error in listening via socket");
    }

    while(1){
      int len = sizeof(address_client);
      client_socket =  accept(server_socket,(struct sockaddr *)&address_client, &len);
      printf("current client socket %d\n",client_socket);
      if(client_socket == -1)
      {
        printf("Error cannot connect to client \n");
      }
      char buffer[256];
      int status = recv(client_socket,buffer,256,0);
      if(status == -1){
        printf("error reading from client \n");
      }

      if(strcmp(buffer,"WRITER")==10){
        all_writers[curr_writers].number = client_socket;
        all_writers[curr_writers].writer_address = address_client;
        pthread_create(&w_pthread[curr_writers],NULL,f_writer,(void *)&all_writers[curr_writers]);
        curr_writers++;
        printf("Current writers %d", curr_writers);
      }
      else if(strcmp(buffer,"READER")==10){
        all_readers[curr_readers].number = client_socket;
        all_readers[curr_readers].reader_address = address_client;
        pthread_create(&r_pthread[curr_readers],NULL,f_reader,(void *)&all_readers[curr_readers]);
        curr_readers++;
        printf("Current readers %d", curr_readers);
      }
      else {
        perror("Not a valid entry ");
      }

    }
    for(int i=0;i<curr_readers;i++){
      pthread_join(r_pthread[i],NULL);
    }
    for(int i=0;i<curr_writers;i++){
      pthread_join(w_pthread[i],NULL);
    }

}
