#include <stdio.h>  
#include <sys/ipc.h>  
#include <sys/shm.h>  
#include <semaphore.h>  
#include <fcntl.h>  
#include <stdlib.h>  
#include <unistd.h>  
#include <string.h>
#include "shm_com_sem.h"

int main()
{
	void* shared_memory = (void*)0;  //缓冲区指针
    struct shared_mem_st *shared_stuff;
    int shm_id;//共享内存id
	pid_t fork_result;
	int flag = 1;
	// 创建key值
	key_t key;
	key = ftok("./ap",4);
	if(key < 0)  // 创建key失败
	{
		perror("fail ftok");
		exit(1);
	}
    shm_id = shmget(key, sizeof(struct shared_mem_st), 0666 | IPC_CREAT);
    //创建共享内存
    if (shm_id < 0) 
	{
        printf("shmget failed\n");
        exit(1);
    }
    if ((shared_memory = shmat(shm_id, 0, 0)) < (void*)0)   //映射共享内存到进程空间
	{
        printf("shmat failed\n");
        exit(1);
    }
    shared_stuff = (struct shared_mem_st*) shared_memory;  //缓冲区指针类型转换
	
	//获取producer的三个信号量
	sem_t *sem_queue, *sem_queue_empty, *sem_queue_full;
    sem_queue = sem_open("queue_mutex", 0);
    sem_queue_empty = sem_open("queue_empty", 0);
    sem_queue_full = sem_open("queue_full", 0);
	
	fork_result = fork();
	
	if(fork_result < 0)
	{
		fprintf(stderr,"Fork failure\n");
	}
	else if (fork_result == 0)  //子进程
	{
		while(1)
		{
			sem_wait(sem_queue_full);
			sem_wait(sem_queue);
			
			printf("This is child process and its id is %d, it gets '%s'\n", getpid(), shared_stuff->buffer[shared_stuff->line_read]);
			//行信息打印
			if (strcmp(shared_stuff->buffer[shared_stuff->line_read], "quit\0") == 0)  //exit退出
			{
				flag = 0;
			}
			strncpy(shared_stuff->buffer[shared_stuff->line_read], "", LINE_SIZE);
			shared_stuff->line_read = (shared_stuff->line_read + 1) % NUM_LINE;
			sem_post(sem_queue);
			sem_post(sem_queue_empty);
			if (flag == 0) 
			{
				break;//退出循环
			}
		}
		sem_unlink(queue_mutex);
		sem_unlink(queue_empty);
		sem_unlink(queue_full);
	}
	else   //父进程
	{
		while (1) 
		{
			sem_wait(sem_queue_full);
			sem_wait(sem_queue);
			printf("This is parent process and its id is %d, it gets '%s'\n", getpid(), shared_stuff->buffer[shared_stuff->line_read]);
			if (strcmp(shared_stuff->buffer[shared_stuff->line_read], "quit\0") == 0) 
			{
				flag = 0;
			}
			strncpy(shared_stuff->buffer[shared_stuff->line_read], "", LINE_SIZE);
			shared_stuff->line_read = (shared_stuff->line_read + 1) % NUM_LINE;
			sem_post(sem_queue);
			sem_post(sem_queue_empty);
			if (flag == 0) 
			{
				break;
			}
		}
		sem_unlink(queue_mutex);
		sem_unlink(queue_empty);
		sem_unlink(queue_full);
	}
	sleep(0.5);
	exit(EXIT_SUCCESS);
}
