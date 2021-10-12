#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

struct my_msgbuf {
   long mtype;
   char mtext[200];
};

int main(int argc, char *argv[]){
    struct my_msgbuf buf;
    int msqid;
    int toend;
    key_t key;
   
    if ((key = ftok("pass.txt", 65)) == -1) {
        perror("ftok");
        exit(1);
    }
   
    if ((msqid = msgget(key, 0600)) == -1) { /* connect to the queue */
        perror("msgget");
        exit(1);
    }
    printf("Queue ready...\n");

    while (1)
    {
        if (msgrcv(msqid, &buf, sizeof(buf.mtext), 0, 0) == -1) {
         perror("msgrcv");
         exit(1);
        }
        printf("Client says: \"%s\"\n", buf.mtext);
        toend = strcmp(buf.mtext,"end");
        if (toend == 0)
        break;
    }
    printf("Queve: Bye...\n");
    return 0;
}