/* File: logfile..c
 * Author: Patrick Carra
 * Class: CS-4760
 * Project 3
 */

#include <time.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "licenseobj.h"
#include "config.h"
#include <errno.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <stdlib.h>


struct sembuf sbl;
int logsemid;

int initsem(){
        //Performs any needed initialization of the license object
        key_t keylog;
        if ((keylog=ftok("./semlog.c", 'B')) == -1){
                perror("Error: creating keylog");
                /* NOTREACHED */
        }
        logsemid = semget(keylog, 1, 0666 | IPC_CREAT);
        sbl.sem_num = 0;
        sbl.sem_op = 1;
        sbl.sem_flg = 0;
        if(semop(logsemid, &sbl, 1)==-1){
                int e = errno;
                if(semctl(logsemid, 0, IPC_RMID) <0){
                        perror("semctl rm");
                }
                errno = e;
                return -1;
        }
        return 0;
}

int cleanuplog(){
        int errnum;
        if (semctl(logsemid, 0, IPC_RMID)==-1){
                errnum = errno;
                perror("semctl remove logsemid");
                errno = errnum;
        }
        return 0;

}

int lock_logsem(int logsemid){
        if(semop(logsemid, &sbl, 1)==1){
                perror("semop lock");
                exit(1);
        }
        return 0;
}

int unlock_logsem(int logsemid){
        sbl.sem_op=1;
        if(semop(logsemid, &sbl, 1)==-1){
                perror("semop unlock");
                exit(1);
        }
        return 0;
}

int logmsg(const char * msg){
        //Write the specified message to the log file.  There is only one log file.
        //This functino will treat the log file as a critical resource.  Append the message and close the file.
	sbl.sem_num=0;
	sbl.sem_op=-1;
	sbl.sem_flg=SEM_UNDO;
	lock_logsem(logsemid);
        //lockup();
        FILE *fp;
        fp = fopen("licenselog.log", "a");
        //returns -1 if filepointer is null
        if(fp==NULL){
                perror("Failed to open log");
                return -1;
        }
        //writes to file
        else {
                fprintf(fp, "%s\n", msg);
                fclose(fp);
                return 0;
        }
        //unlock();
	unlock_logsem(logsemid);
        return 0;
}
