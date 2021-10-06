/* File: licenseobj.h
 * Author: Patrick Carra
 * Class: CS-4760
 * Project 3
 */

#include <time.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/sem.h>


struct License {
        int nlicenses;
	int semid;
};


int getlicense(void);
int returnlicense(void);
int initlicense(int number);
int addtolicenses(int n);
int removelicenses(int n);
int cleanuplicense();
