#include <stdio.h>
#include <malloc.h>
#include <unistd.h>

int main()
{
	printf("This is process %d\n",getpid());

	printf("Before malloc()s.\n");
 	getchar();

 	int *bufs[8];

 	bufs[0] = (int *)malloc(1024*1024*32*sizeof(int));  //1  (2^10)*(2^10)*(2^5)*(2^2)=2^27=(2^7)*(2^20)=128M
 	bufs[1] = (int *)malloc(1024*1024*32*sizeof(int));  //2
 	bufs[2] = (int *)malloc(1024*1024*32*sizeof(int));  //3
 	bufs[3] = (int *)malloc(1024*1024*32*sizeof(int));  //4
 	bufs[4] = (int *)malloc(1024*1024*32*sizeof(int));  //5
 	bufs[5] = (int *)malloc(1024*1024*32*sizeof(int));  //6
 	
 	printf("After 6 malloc()s.\n");
 	getchar();

 	free(bufs[1]);  //free 2
 	printf("2 is freed\n");
 	getchar();

 	free(bufs[2]);  //free 3
 	printf("3 is freed\n");
 	getchar();

 	free(bufs[4]);  //free 5
 	printf("5 is freed\n");
 	getchar();

 	printf("After 3 free()s.\n");
 	getchar();

 	bufs[6] = (int *)malloc(1024*1024*256*sizeof(int));
 	printf("1024M allocated\n");
	getchar();

 	bufs[7] = (int *)malloc(1024*1024*16*sizeof(int));
 	printf("64M allocated\n");
 	getchar();

 	free(bufs[0]);  //free 1
 	free(bufs[3]);  //free 4
 	free(bufs[5]);  //free 6
 	free(bufs[6]);  //free 7
 	free(bufs[7]);  //free 8
 	return 0;
}