#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <error.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <ctype.h>
#include <termios.h>

#define SERVER_QUEUE_NAME   "/sp-server"
#define QUEUE_PERMISSIONS 0660
#define MAX_MESSAGES 10
#define MAX_MSG_SIZE 256
#define MSG_BUFFER_SIZE MAX_MSG_SIZE + 10

pthread_mutex_t keyQueue;

struct events{
    char event_name[50];
};

struct client{
    char client_id[50];
    struct events clientsEvents;
}clients[10];

char *buffer;
char comando[100];
    
mqd_t qd_server, qd_client;   // queue descriptors
struct mq_attr attr;
char in_buffer [MSG_BUFFER_SIZE];
char in_buffer0 [MSG_BUFFER_SIZE];
    
void funcExit();
void funcAdd(char *buffer);
void funcRemove(char *buffer);
void funcTrigger(char *buffer);
void funcList();
void funcResponse();

void funcSub(char *buffer);
void funcUnsub(char *buffer);
void funcListC();
void funcAsk();

void *readCommands(void *arg);
void *readReceiver(void *arg);

void *readCommands(void *arg){
    while (1)
    {
        fgets(comando,sizeof(comando),stdin);
        for (int i = 0; comando[i] !='\0'; ++i)
        {
            comando[i] = tolower(comando[i]);
        }
        char *token = strtok(comando," ");
        char *action = NULL;
        char *resp;
        for (int i = 0; token!= NULL; i++)
        {
            if (i == 0)
            {
                action = token;
            }else if (i == 1)
            {
                resp = token;
            }
            token = strtok(NULL," ");
        }

        int exit = strcmp(action,"exit\n");
        int add = strcmp(action,"add");
        int remove = strcmp(action,"remove");
        int trigger = strcmp(action,"trigger");
        int list = strcmp(action,"list\n");

        if (exit == 0)
        {
            funcExit();
        }else if (add == 0)
        {
            funcAdd(resp);
        }else if (remove == 0)
        {
            funcRemove(resp);
        }else if (trigger == 0)
        {
            funcTrigger(resp);
        }else if (list == 0)
        {
            funcList();
        }
    }
    return NULL;
}

void *readReceiver(void *arg){
    while (1)
    {
        pthread_mutex_lock(&keyQueue);
        if ((mq_receive (qd_server, in_buffer, MSG_BUFFER_SIZE, NULL))!=-1 && (mq_receive (qd_server, in_buffer0, MSG_BUFFER_SIZE, NULL))!=-1)
            {
                printf ("Server: message received: %s\n",in_buffer);
                printf ("Server: message received: %s\n",in_buffer0);
                char *token = strtok(in_buffer0," ");
                char *action = NULL;
                char *resp;
                for (int i = 0; token!= NULL; i++)
                {
                    if (i == 0)
                    {
                        action = token;
                    }else if (i == 1)
                    {
                        resp = token;
                    }
                    token = strtok(NULL,"_");
                }

                int sub = strcmp(action,"sub");
                int unsub = strcmp(action,"unsub");
                int list = strcmp(action,"list\n");
                int ask = strcmp(action,"ask\n");
                
                if (sub == 0)
                {
                    funcSub(resp);
                }else if (unsub == 0)
                {
                    funcUnsub(resp);
                }else if (list == 0)
                {
                    funcListC();
                }else if (ask == 0)
                {
                    funcAsk();
                }                 
            }
        }
        pthread_mutex_unlock(&keyQueue);
        
    return NULL;
}
void funcResponse(){
    printf("Response: \n");
    if ((qd_client = mq_open (in_buffer, O_WRONLY))!=1 && (mq_send (qd_client, comando, sizeof(comando), 0))!=-1)
    {
        printf ("Server: response sent to client.\n");
    }else{
        perror("Error en FuncResponse: ");
    } 
}


int main (int argc, char **argv)
{
    attr.mq_flags = 0;
    attr.mq_maxmsg = MAX_MESSAGES;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;

    if ((qd_server = mq_open (SERVER_QUEUE_NAME, O_RDONLY | O_CREAT, QUEUE_PERMISSIONS, &attr)) == -1) {
        perror ("Server: mq_open (server)");
        exit (1);
    }
    
    printf ("Server: Hi, welcome to Juan & Hugo's server!\n");
    
    pthread_mutex_init(&keyQueue,NULL);
    pthread_t threads1,threads2;
    pthread_create(&threads1,NULL,&readCommands,NULL);
    pthread_create(&threads2,NULL,&readReceiver,NULL);
    pthread_join(threads1,NULL);
    pthread_join(threads2,NULL);

    return(EXIT_SUCCESS);
}

void funcExit(){
    funcTrigger("exit");
    exit(0);
}
void funcAdd(char *resp){
    for (int i = 0; i < 10; i++)
    {
        if (strcmp(clients[i].clientsEvents.event_name,"") == 0)
        {
            strcpy(clients[i].clientsEvents.event_name, resp);
            printf("Agrega %d: %s",i,clients[i].clientsEvents.event_name);
            break;
        }
    }
}
void funcRemove(char *resp){
    for (int i = 0; i < 10; i++)
    {
        if (strcmp(clients[i].clientsEvents.event_name, resp) == 0)
        {
            strcpy(clients[i].clientsEvents.event_name,"");
            printf("se elimino el evento \n" );
            break;
        }
    }
}
void funcTrigger(char *resp){
    if ((qd_client = mq_open (in_buffer, O_WRONLY)) == 1) {
        perror ("Server: Not able to open client queue");
    }
    if (mq_send (qd_client, resp, sizeof(resp), 0) == -1) {
        perror ("Server: Not able to send message to client");
    }
    printf ("Server: response sent to client.\n");
}
void funcList(){
    for (int i = 0; i < 10; i++)
    {
        printf("Eventos %d: %s\n",i,clients[i].clientsEvents.event_name);
    }
}
void funcSub(char *resp){
    for (int i = 0; i < 10; i++)
    {
        if (strcmp(clients[i].clientsEvents.event_name,"") == 0)
        {
            strcpy(clients[i].clientsEvents.event_name, resp);
            printf("Agrega %d: %s",i,clients[i].clientsEvents.event_name);
            break;
        }
        //funcResponse();
    }
}
void funcUnsub(char *resp){
    for (int i = 0; i < 10; i++)
    {
        if (strcmp(clients[i].clientsEvents.event_name, resp) == 0)
        {
            strcpy(clients[i].clientsEvents.event_name,"");
            printf("se elimino el evento \n" );
            break;
        }
        //funcResponse();
    }
}
void funcListC(){
    for (int i = 0; i < 10; i++)
    {
        printf("Eventos Cliente %d: %s\n",i,clients[i].clientsEvents.event_name);
        //funcResponse();
    }
}
void funcAsk(){
    for (int i = 0; i < 10; i++)
    {
        printf("Eventos %d: %s\n",i,clients[i].clientsEvents.event_name);
        //funcResponse();
    }
}

