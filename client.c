#include <stdio.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

struct my_msgbuf
{
    long mtype;
    char mtext[200];
};

int main(int argc, char *argv[]){
    // Lectura de argumentos argv    
    for (int i = 0; i < argc; i++)
    {
        printf("Argv[%d]: %s\n",i,argv[i]);
    }
    // Variables
    struct my_msgbuf buf;
    int msqid;
    int len;
    key_t key;
    system("touch pass.txt");

    if ((key = ftok("pass.txt", 65)) == -1) {
        perror("ftok");
        exit(1);
    }
   
    if ((msqid = msgget(key, 0600 | IPC_CREAT)) == -1) {
        perror("msgget");
        exit(1);
    }
    printf("Queve Ready.\n");
    printf("Enter lines of text, Ctrl + D to quit:\n");
    while(fgets(buf.mtext, sizeof buf.mtext, stdin) != NULL) {
        len = strlen(buf.mtext);
        /* remove newline at end, if it exists */
        if (buf.mtext[len-1] == '\n') buf.mtext[len-1] = '\0';
        if (msgsnd(msqid, &buf, len+1, 0) == -1) /* +1 for '\0' */
            perror("msgsnd");
    }
    strcpy(buf.mtext, "end");
    len = strlen(buf.mtext);
    if (msgsnd(msqid, &buf, len+1, 0) == -1) /* +1 for '\0' */
    perror("msgsnd");
    
    if (msgctl(msqid, IPC_RMID, NULL) == -1) {
        perror("msgctl");
        exit(1);
    }
    printf("Queve Done Sending messages.\n");
    system("rm pass.txt");
    return 0;

}