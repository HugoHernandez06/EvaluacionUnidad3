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

void funcExit(char *buffer);
void funcAdd(char *buffer);
void funcRemove(char *buffer);
void funcTrigger(char *buffer);
void funcList(char *buffer);

int main (int argc, char **argv)
{
    int fd, fd_client, bytes_read;
    char buf [4096];
    char bufCmd [100];
    char *return_fifo;

    if ((mkfifo (SERVER_FIFO, 0664) == -1) && (errno != EEXIST)) {
        perror ("mkfifo");
        exit (1);
    }
    if ((fd = open (SERVER_FIFO, O_RDONLY)) == -1){
        perror ("open");
    }
    printf("Welcome to Juan & Hugo's server =D\n\n"); 

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

void funcExit(char *buffer){
    printf("Soy Exit");
}
void funcAdd(char *buffer){
    printf("Soy Add");
}
void funcRemove(char *buffer){
    printf("Soy Remove");
}
void funcTrigger(char *buffer){
    printf("Soy Trigger");
}
void funcList(char *buffer){
    printf("Soy List");
}