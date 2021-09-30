/* File: testsim.c
 * Author: Patrick Carra
 * Class: CS-4760
 * Project 2
 */

#include <time.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>


struct License {
        int nlicenses;
	bool choosing[20];
	int number[20];
};


int getlicense(void);
int returnlicense(void);
int initlicense(int number);
int addtolicenses(int n);
int removelicenses(int n);
//int logmsg(const char * msg, int i);
int logmsg(const char * msg);

