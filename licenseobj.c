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
#include <stdbool.h>

struct License *license;


int getlicense(){
	//Blocks until a license is available
	if(license->nlicenses>0){
		(license->nlicenses)--;
	}
	else{
		wait(&license->nlicenses);
		//printf("Blocking for license....\n");
		return 0;
	}
	return 0;
}

int returnlicense(){
	//Increments the number of available licenses
	(license->nlicenses)++;
	return 0;
}

int initlicense(int number){
	//Performs any needed initialization of the license object
	license->nlicenses = number;
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
	/*
	 * bakery algorithm attempt
	do {
		license->choosing[i] = true;
		int num_max;
		for(int k=0; k<19; k++){
			num_max = max(license->number[k], license->number[k+1]);
		}
		license->number[i]= num_max;
		for (int j=0; j<20; j++){
			while (license->choosing[j]);
			while ((license->number[j]) && (license->number[j], j) < (license->number[i], i));
		}
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
		license->number[i]=0;
		return 0;
	} while(1);
	*/
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
	return 0;
}

int max(int num1, int num2){
	if(num1>num2)
		return num1;
	else
		return num2;
}

