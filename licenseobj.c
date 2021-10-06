/* File: licenseobj.c
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

struct License *license;
struct sembuf sb;
extern int errno;

int cleanuplicense(int semid){
	int errnum;
        if (semctl(semid, 0, IPC_RMID)==-1){
		errnum = errno;
		perror("semctl remove semid");
		errno = errnum;
	}
	return 0;

}

int lock_sem(int semid){
	if(semop(semid, &sb, 1)==1){
		perror("semop lock");
		exit(1);
	}
	return 0;
}

int unlock_sem(int semid){
	sb.sem_op=1;
	if(semop(semid, &sb, 1)==-1){
		perror("semop unlock");
		exit(1);
	}
	return 0;
}

int getlicense(){
	sb.sem_num=0;
	sb.sem_op=-1;
	sb.sem_flg=SEM_UNDO;
	//Blocks until a license is availablea
	lock_sem(license->semid);
	if(license->nlicenses>0){
		(license->nlicenses)--;
	}
	unlock_sem(license->semid);
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
