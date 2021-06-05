#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
int main ()
{
	int i;

	printf("This is process %d\n",getpid());
	getchar();

	printf("Before allocating...\n");
	getchar();

	char *buf = (char *)malloc(1024*1024*256*sizeof(char));  //256 Megabit
	printf("After allocating...\n");
	getchar();


	printf("Now we start to write!\n");
	getchar();

	//WRITE action
	for(i=0; i<1024*1024*256; i+=(4*1024))
	{
		buf[i] = '1';
	}
	printf("Writing Finish!\n");
	getchar();

/*
	printf("Now we start to read!\n");
	getchar();

	//READ action
	for(i=0; i<1024*1024*256; i+=(4*1024))
	{
		buf[i];
	}
	printf("Reading finish!\n");
	getchar();*/
}