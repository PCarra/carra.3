/* File: runsim.c
 * Author: Patrick Carra
 * Class: CS-4760
 * Project 2
 */

#include <stdlib.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/time.h>
#include <errno.h>
#include <string.h>
#include "config.h"
#include "licenseobj.h"

#define MAX_CANON 20
#define IPC_RESULT_ERROR (-1)
#define BUF_SIZE 1024
#define IP_RESULT_ERROR 1

//declared in licenseobj.c
extern struct License *license;

//input integer and void pointer
//detach shared memory
//output success status
int detachandremove(int shmid, void *shmaddr){
	int error=0;
	if (shmdt(shmaddr)==-1)
		error = errno;
	if ((shmctl(shmid, IPC_RMID, NULL)==-1) && !error)
		error = errno;
	if (!error)
		return 0;
	errno = error;
	return -1;
}

//print usage for command line parameters and program execution
void print_usage(char *argv[]){
	fprintf(stderr, "Usage: %s [number of processes]\n", argv[0]);
}

//inputs parse testing.data parameters
//execute testsim and wait on grandchildren to exit
void docommand(char *cline){
	//fork a child (a grandchild of the original), Grandchild calls makeargv on cline and calls execvp on the resulting array
	pid_t childpid = 0;
 	int grchild_count = 0;
	char errstr[255];
	getlicense();
	//printf("License count: %d\n", license->nlicenses);
	if((childpid=fork())<0){
		printf(errstr, "Error: Failed to create child process");
		perror(errstr);
		exit(1);
        }
	else if(childpid==0){
		grchild_count++;
		getlicense();
		char delim[] = " ";
		char *binaryPath = strtok(cline, delim);
		char *arg1 = strtok(NULL, delim);
		char *arg2 = strtok(NULL, delim);
		char *arg3 = strtok(NULL, delim);
		/* attempted bakery algorithm could not figure out per program instructions they are vague and lack critical examples
		//char *arg4 = strtok(NULL, delim);
		//execl(binaryPath, binaryPath, arg1, arg2, arg3, arg4, NULL);
		for (int i=0; i<atoi(arg3); i++){
			execl(binaryPath, binaryPath, arg1, arg2, arg3, NULL);
		}
		*/
		execl(binaryPath, binaryPath, arg1, arg2, arg3, NULL);
	}
	//runsim checks to see if any of the children have finished waitpid with WNOHANG option) and when that happens it returnlicense
	else{
		if ((childpid=waitpid(-1,NULL, WNOHANG)) == -1){
			printf("Waiting on grand child to exit...\n");
               		grchild_count--;
			//waits on children
        	}
		//wait for this child and then return license to the license object
		returnlicense();
	}
}

//input signal integer
//catches signal prints output and exits
void INThandler(int sig){
	printf( "Caught signal %d exiting....\n", sig);
	signal(SIGQUIT, SIG_IGN);
	kill(0, SIGTERM);
	time_t current_time;
    	struct tm * time_info;
	char timeString[9];
	time_info = localtime(&current_time);
	strftime(timeString, sizeof(timeString), "%H:%M:%S", time_info);
	strcat(timeString, " Exiting runsim");
	logmsg(timeString);
	exit(0);
}

//input integer s from book example but appears unused?
//handler for signal ctrl-c interrupt
static void myhandler(int s){
	int errsave;
	errsave = errno;
	write(STDERR_FILENO, "Exceeded time limit....", 1);
	raise(SIGINT);
	errno = errsave;
}

//interrupt function per book example
static int setupinterrupt(void){
	struct sigaction act;
	act.sa_handler = myhandler;
	act.sa_flags=0;
	return (sigemptyset(&act.sa_mask) || sigaction(SIGPROF, &act, NULL));
}

//timer function per book example
static int setupitimer(void){
	struct itimerval value;
	value.it_interval.tv_sec = MAX_SECONDS;
	value.it_interval.tv_usec = 0;
	value.it_value = value.it_interval;
	return (setitimer(ITIMER_PROF, &value, NULL));
}

int main (int argc, char *argv[]) {
	char errstr[255];
	int num_proc;
	//setup signal handler
	signal(SIGINT, INThandler);
	if (setupinterrupt() == -1){
		printf(errstr, "%s: Error: Failed to set up handler for SIGPROF", argv[0]);
		perror(errstr);
		return 1;
	}
	if (setupitimer() == -1){
		printf(errstr, "%s: Error: Failed to set up ITIMER_PROF interval timer", argv[0]);
		perror(errstr);
		return 1;
	}
	//set childpid to 0 for parent
	pid_t childpid = 0;
	int shmid, pr_count;//specifies the maximum number of children allowed to execute at a time (int value passed in on cmd line)

	//check command line arguments and set default 
	if (argc != 2){  //check for valid number of command-line arguments 
		print_usage(argv);
 		return 1;
 	}
	else if (argc == 2)
		num_proc = atoi(argv[1]);
	else
		num_proc = 5;

	//get an id to the shared segment
	if ((shmid=shmget(IPC_PRIVATE, sizeof(int), 0666 | IPC_CREAT))==-1){
		printf(errstr, "%s: Error: Failed to create shared memory segment", argv[0]);
		perror(errstr);
		return 1;
	}
	//allocate a size of struct licenses to the pointer nlicenses and attach the shared segment to the pointer
	if ((license = (struct License *)shmat(shmid, NULL, 0)) == (void *)-1){
		printf(errstr, "%s: Error: Failed to attach shared memory segment", argv[0]);
		perror(errstr);
		if(shmctl(shmid, IPC_RMID, NULL)==-1){
			printf(errstr, "%s: Error: Failed to remove memory segment", argv[0]);
			perror(errstr);
		}
		return 1;
	}

	//populate shared memory with command line argument for the number of available licenses
	initlicense(num_proc);
	license->number[0]=0;
	printf("License count: %d\n", license->nlicenses);
	//request a license from the license object
	getlicense();
	pr_count=0;
	char buffer[MAX_CANON];
	//read from stdin
	while(fgets(buffer, sizeof buffer, stdin)!=NULL){
                //waits for a child process to finish if the limit is reached
                if(pr_count==NUM_PROCESSES){
                	childpid = wait(NULL);
			if(childpid!=-1){
				printf(errstr, "%s: Error: Waited for child with pid %d because max processes reached", argv[0], (long)getpid(), childpid);
				perror(errstr);
			}
                        pr_count--;
                }
		//printf("License count: %d\n", license->nlicenses);
                //fork a child that calls docommand
		if((childpid=fork())<0){
			printf(errstr, "%s: Error: Failed to create child process", argv[0]);
			perror(errstr);
			if(detachandremove(shmid, license)==-1){
				printf(errstr, "%s: Error: Failed to destroy shared memory segment", argv[0]);
				perror(errstr);
			}
			exit(1);
                }
		else if(childpid==0){
			if ((license = (struct License *)shmat(shmid, NULL, 0)) == (struct License *)-1){
				printf(errstr, "%s: Error: Failed to attach shared memory segment", argv[0]);
				perror(errstr);
				if(shmctl(shmid, IPC_RMID, NULL)==-1){
					printf(errstr, "%s: Error: Failed to remove memory segment", argv[0]);
					perror(errstr);
				}
				return 1;
			}
			pr_count++;
			license->number[pr_count]=pr_count;
			//printf("I am a child %ld\n", (long)getpid());
                	//pass the input string from stdin to docommand which will execl the command (child)
                        char progstr[20];
                        strcpy(progstr, "./");
                        strcat(progstr, buffer);
			//strcat(progstr, license->number[pr_count]);
                        docommand(progstr);
			break;
		}
		else{
			//parent waits on children
			//printf("I am a parent %ld\n", (long)getpid());
			if((childpid=waitpid(-1,NULL, WNOHANG))==-1){
				//waits on children
                		pr_count--;
                	}
		}
	}
	time_t current_time;
    	struct tm * time_info;
	char timeString[9];
	time_info = localtime(&current_time);
	strftime(timeString, sizeof(timeString), "%H:%M:%S", time_info);
	strcat(timeString, " Exiting runsim");
	logmsg(timeString);
	//detach memory
	if(detachandremove(shmid, license) == -1) {
		printf(errstr, "%s: Error: Failed to destroy shared memory segment", argv[0]);
		perror(errstr);
		return 1;
	}
	return 0;
}
