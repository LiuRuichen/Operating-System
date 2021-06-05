#include <stdio.h>  
#include <stdlib.h>  
#include <unistd.h>  
int main ()  
{  
	printf("This is process %d\n",getpid());  
	getchar();  
	  
	char *buf[4000];  
	for(int i=0; ; i++)  
	{
		buf[i] = (char *)malloc(1024*1024*sizeof(char));  //1 Megabit  
	    if (!buf[i])  
	    {  
	        printf("No memory left! Exiting...\n");  
	        getchar();  
	        break;  
	    }  
	    else  
	    {  
	        printf("This is the %d-time-allocation! Going on...\n",i+1);
	        for(int j=0; j<1024*1024; j++)  //写入  
	        {  
	            buf[i][j] = '1';  
	        }  
	    }  
	}  
}  
