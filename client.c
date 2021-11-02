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

char client_queue_name [64];
mqd_t qd_server, qd_client;   // queue descriptors
struct mq_attr attr;
char in_buffer [MSG_BUFFER_SIZE]; // ID por Client
char in_buffer0 [MSG_BUFFER_SIZE]; // msg

void funcSender(char *resp);

void *readCommands(void *arg);
void *readReceiver(void *arg);

char comando[100];

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
            token = strtok(NULL,"_");
        }

        int msg = strcmp(action,"m:");
        if (msg == 0)
        {
            funcSender(resp);
        }
    }
    return NULL;
}

void *readReceiver(void *arg){
    // create the client queue for receiving messages from server
    
    if ((qd_client = mq_open (client_queue_name, O_RDONLY | O_CREAT, QUEUE_PERMISSIONS, &attr))!=-1)
    {
        while (1)
        {
            pthread_mutex_lock(&keyQueue);
            // receive response from server
            if (mq_receive (qd_client, in_buffer, MSG_BUFFER_SIZE, NULL) == -1) {
                perror ("Client: mq_receive");
            }
            // display token received from server
            printf ("Client: message received from server: %s\n", in_buffer);
            pthread_mutex_unlock(&keyQueue);
        }
    }
    return NULL;
}


int main (int argc, char **argv)
{
    // create the client queue for receiving messages from server
    sprintf (client_queue_name, "/sp-client-%d", getpid ());
    
    pthread_t threadsCmd,threadsReceiver;
    pthread_mutex_init(&keyQueue,NULL);
    pthread_create(&threadsCmd,NULL,&readCommands, NULL);
    pthread_create(&threadsReceiver,NULL,&readReceiver,NULL);
    pthread_join(threadsCmd,NULL);
    pthread_join(threadsReceiver,NULL);
    
    attr.mq_flags = 0;
    attr.mq_maxmsg = MAX_MESSAGES;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;
    
    return(EXIT_SUCCESS);
}

void funcSender(char *resp){
    char temp_buf [100];
    if ((qd_server = mq_open (SERVER_QUEUE_NAME, O_WRONLY))!=-1)
    {
        // send message to server
        if ((mq_send (qd_server, client_queue_name, strlen (client_queue_name) + 1, 0)) == -1) {
            perror ("Client: Not able to send message to server");
        }
        if ((mq_send (qd_server, resp, sizeof(resp), 0)) == -1) {
            perror ("Client: Not able to send message to server");
        }
    }else{
        perror("Error al enviar: ");
    }
}