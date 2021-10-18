#include <stdio.h>
#include <stdlib.h>
#include <error.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <string.h>
#include <ctype.h>
#include <termios.h>

#define SERVER_FIFO "/tmp/addition_fifo_server"

typedef struct{
    char event_name[100];
    char clients[100];
}events;

char event[10][10] = {"null","null","null","null","null","null","null","null","null","null"};
char *buffer;
char comando[100];

int fd, fd_client, bytes_read;
char buf [4096];
char bufCmd [100];
char *return_fifo;

void funcExit();
void funcAdd(char *resp);
void funcRemove(char *resp);
void funcTrigger(char *resp);
void funcList();
void *threadComandos(void *arg);

void *threadComandos(void *arg){
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

int main (int argc, char **argv)
{
    pthread_t threadsID;
    pthread_create(&threadsID,NULL,&threadComandos, NULL);
    if ((mkfifo (SERVER_FIFO, 0664) == -1) && (errno != EEXIST)) {
        perror ("mkfifo");
        exit (1);
    }
    if ((fd = open (SERVER_FIFO, O_RDONLY)) == -1){
        perror ("open");
    }else {
        printf("Welcome to Juan & Hugo's server =D\n\n"); }
    
    while (1) {
        // Get a message
        memset (buf, '\0', sizeof (buf));
        if ((bytes_read = read (fd, buf, sizeof (buf))) == -1)
            perror ("read");
        if (bytes_read == 0){
            continue;
        }
        
        printf("Message of Cliente: %s",buf);

        if (bytes_read > 0) {
            return_fifo = strtok (buf, ", \n");
            /* Send the result */
            if ((fd_client = open (return_fifo, O_WRONLY)) == -1) {
                perror ("open: client fifo");
                continue;
            }   
                printf("Console: ");
                fgets(bufCmd,30,stdin);
                printf("\n");
                sprintf (buf, "Response of Server:  %s\n", bufCmd);

            if (write (fd_client, buf, strlen (buf)) != strlen (buf))
                perror ("write");

            if (close (fd_client) == -1)
                perror ("close");
        } 
    }
    
    return(EXIT_SUCCESS);
}

void funcExit(){
    funcTrigger("exit");
    exit(0);
}
void funcAdd(char *resp){
    for (int i = 0; i < 10; i++)
    {
        if (strcmp(event[i],"null") == 0)
        {
            strcpy(event[i], resp);
            printf("Agrega %d: %s",i,event[i]);
            break;
        }
    }
}
void funcRemove(char *resp){
    printf("Soy Remove\n");
}
void funcTrigger(char *resp){
    printf("Soy Trigger\n");
    if (bytes_read > 0) {
            return_fifo = strtok (resp, ", \n");
            /* Send the result */
            if ((fd_client = open (return_fifo, O_WRONLY)) == -1) {
                perror ("open: client fifo");
            }   
                printf("Console: ");
                fgets(bufCmd,30,stdin);
                printf("\n");
                sprintf (resp, "Response of Server:  %s\n", bufCmd);

            if (write (fd_client, resp, strlen (resp)) != strlen (resp))
                perror ("write");

            if (close (fd_client) == -1)
                perror ("close");
        } 
}
void funcList(){
    for (int i = 0; i < 10; i++)
    {
        printf("Eventos %d: %s\n",i,event[i]);
    }
}