/* File: licenseobj.c
 * Author: Patrick Carra
 * Class: CS-4760
 * Project 2
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

struct License *license;
union semun {
	int val;
	struct semid_ds *buf;
	ushort *array;
};

struct sembuf sb;
/*
if (semop(id, sem_lock, 1) < 0 ){
//	//error handling code
}
*/
/*
if (semop(id, sem_unlock, 1) < 0){
	//error handling code
}
*/


int cleanuplicense(){
	int error = 0;
        if ((semctl(license->semid, 0, IPC_RMID)==-1) && !error)
                error = errno;
        if ((semctl(license->logsemid, 0, IPC_RMID)==-1) && !error)
                error = errno;
	return 0;

}

int getlicense(){
	sb.sem_num=0;
	sb.sem_op=-1;
	sb.sem_flg=0;
	//Blocks until a license is availablea
	printf("inside getlicense func\n");
	if(semop(license->semid, &sb, 1)==-1){
		perror("semop");
		exit(1);
	}
	printf("getlicense locked the sem\n");
	if(license->nlicenses>0){
		(license->nlicenses)--;
	}
	sb.sem_op=1;
	if(semop(license->semid, &sb, 1)==-1){
		perror("semop");
		exit(1);
	}
	printf("getlicense unlocked the sem\n");
	return 0;
}

int returnlicense(){
	//Increments the number of available licenses
	(license->nlicenses)++;
	return 0;
}

int initlicense(int number){
	//Performs any needed initialization of the license object
	key_t keyid;
	key_t keylog;
	license->nlicenses = number;
	if ((keyid=ftok("./semfuncs.c", 'J')) == -1){
	       	perror("Error: creating keyid");
		/* NOTREACHED */
	}
	if ((keylog=ftok("./semlog.c", 'B')) == -1){
		perror("Error: creating keylog");
		/* NOTREACHED */
	}
	license->semid = semget(keyid, 1, 0666 | IPC_CREAT);
	license->logsemid = semget(keylog, 1, 0666 | IPC_CREAT);
	sb.sem_num = 0;
	sb.sem_op = 1;
	sb.sem_flg = 0;
	if(semop(license->semid, &sb, 1)==-1){
		int e = errno;
		if(semctl(license->semid, 0, IPC_RMID) <0){
			perror("semctl rm");
		}
		errno = e;
		return -1;
	}
	return 0;
}

int addtolicenses(int n){
	//Adds n licenses to the number available
	license->nlicenses+=n;
	return 0;
}

int removelicenses(int n){
	//Decrements the number of licenses by n
	license->nlicenses-=n;
	return 0;
}

//int logmsg(const char * msg, int i){
int logmsg(const char * msg){
	//Write the specified message to the log file.  There is only one log file.
	//This functino will treat the log file as a critical resource.  Append the message and close the file.
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
	return 0;
}

