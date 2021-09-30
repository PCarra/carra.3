#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int main(int argc, char *argv[]){
	int delet_shm = 0;
	if(argc>1){
		//delete_shm = atoi(argv[1]);
	}
	printf("This program was executed as: ");
	for(int i=0; i<argc; i++)
		printf("%s ", argv[i]);
	printf("\n\n");

	int key = 99999;
	int segment_id;

	segment_id = shmget(key, sizeof(int), IPC_CREAT | 0666);

	if(segment_id == -1)
		perror("shmget: ");
	printf("My segment id is %d\n", segment_id);

}
