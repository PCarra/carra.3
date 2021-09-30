/* File: testsim.c
 * Author: Patrick Carra
 * Class: CS-4760
 * Project 2
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include "licenseobj.h"

extern struct License *license;


int main(int argc, char *argv[]) {
	time_t current_time;
	struct tm * time_info;
	char timeString[9];

	int sleep_time = atoi(argv[1]);
    	int repeat_factor = atoi(argv[2]);
	//attempted bakery algorithm needed i for bakery
	//int numi = atoi(argv[3])
    	if (argc <=2){
	  	fprintf(stderr, "Usage: %s sleep_time repeat_factor\n", argv[1]);
		    exit(1);
    	}
	//execute loop with sleep and repeat
    	for(int i=1; i<repeat_factor; i++){
		char str[10];
		time(&current_time);
		time_info = localtime(&current_time);
		strftime(timeString, sizeof(timeString), "%H:%M:%S", time_info);
		sleep(sleep_time);
		sprintf(str, " %d", getpid());
		strcat(timeString, str);
    	    	//printf("%s %d %d\n", timeString, getpid(), i);
		//bakery algorithm attempt
		//logmsg(timeString, numi);
    	    	//Ouptut to logfile in format:Time PID Iteration# of NumberOfIterations
		logmsg(timeString);
    	}
    	return sleep_time;
}
