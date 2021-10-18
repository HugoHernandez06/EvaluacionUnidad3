#include <stdio.h>
#include <stdlib.h>
#include <error.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <memory.h>
#include <pthread.h>
#include <string.h>
#include <ctype.h>
#include <termios.h>

#define SERVER_FIFO "/tmp/addition_fifo_server"
char comando[100];
char my_fifo_name [128];
char buf1 [512], buf2 [1024];

void funcSub(char *buffer);
void funcUnsub(char *buffer);
void funcListC(char *buffer);
void funcAsk(char *buffer);
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

        int sub = strcmp(action,"sub\n");
        int unsub = strcmp(action,"unsub");
        int list = strcmp(action,"list\n");
        int ask = strcmp(action,"ask");

        if (sub == 0)
        {
            funcSub(resp);
        }else if (unsub == 0)
        {
            funcUnsub(resp);
        }else if (list == 0)
        {
            funcList(resp);
        }else if (ask == 0)
        {
            funcAsk(resp);
        }
    }
    return NULL;
}

int main (int argc, char *argv[])
{
    pthread_t threadsID;
    pthread_create(&threadsID,NULL,&threadComandos, NULL);

    int fd, fd_server, bytes_read;

    // make client fifo
    sprintf (my_fifo_name, "/tmp/add_client_fifo%ld", (long) getpid ());

    if (mkfifo (my_fifo_name, 0664) == -1){
       perror ("mkfifo");
    }
        
    while (1) {
        // get user input
        printf ("Message: ");
        if (fgets(buf1, sizeof (buf1), stdin) == NULL)
            break;

        strcpy (buf2, my_fifo_name);
        strcat (buf2, " ");
        strcat (buf2, buf1);
        // send message to server

        if ((fd_server = open (SERVER_FIFO, O_WRONLY)) == -1) {
            perror ("open: server fifo");
            break;
        }

        if (write (fd_server, buf2, strlen (buf2)) != strlen (buf2)) {
            perror ("write");
            break;
        }

        if (close (fd_server) == -1) {
            perror ("close");
            break;
        }

        // read the answer
        if ((fd = open (my_fifo_name, O_RDONLY)) == -1)
           perror ("open");
        memset (buf2, '\0', sizeof (buf2));
        if ((bytes_read = read (fd, buf2, sizeof (buf2))) == -1)
            perror ("read");

        if (bytes_read > 0) {
            printf ("%s\n", buf2);
        }

        if (close (fd) == -1) {
            perror ("close");
            break;
        }
    }
}
void funcSub(char *buffer){
    printf("Soy sub");
}
void funcUnsub(char *buffer){
    printf("Soy unsub");
}
void funcListC(char *buffer){
    printf("Soy List");
}
void funcAsk(char *buffer){
    printf("Soy Ask");
}

