#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <semaphore.h>
#include "shm_com_sem.h"

int main()
{
	void *shared_memory = (void*)0;  //缓冲区指针
    struct shared_mem_st *shared_stuff;
	char key_line[256];//接收读入的字符，最大不超过LINE_SIZE
    int shm_id;//共享内存id
	
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
	
    //创建信号量,分别为访问共享内存的互斥量、空缓冲区、满缓冲区信号量
	sem_t *sem_queue, *sem_queue_empty, *sem_queue_full;
    sem_queue = sem_open("queue_mutex", O_CREAT, 0644, 1);
    sem_queue_empty = sem_open("queue_empty", O_CREAT, 0644, 16);
    sem_queue_full = sem_open("queue_full", O_CREAT, 0644, 0);
	
	char c;
	int i;
	int flag = 1;//退出循环读取字符串的标志
    //初始化读写指针
    shared_stuff->line_write = 0;
    shared_stuff->line_read = 0;
    while (1) 
	{
        printf("Enter your text('quit' for exit):");//输入提示
		
		i = -1;
		while((c = getchar())!='\n')
		{
			key_line[++i] = c;
		}
		key_line[++i] = '\0';
        sem_wait(sem_queue_empty);        //等待信号量输入
        strncpy(shared_stuff->buffer[shared_stuff->line_write], key_line, LINE_SIZE);   //将reader的值写进
        //将行信息保存在缓冲区中
        if (strcmp(shared_stuff->buffer[shared_stuff->line_write], "quit\0") == 0)   //输入quit则退出
		{
            flag = 0;
        }
        shared_stuff->line_write = (shared_stuff->line_write + 1) % NUM_LINE;   //写指针增加，多了则从头开始
        sem_post(sem_queue_full);  //发送信号量
        if (!flag) 
		{
			break;
		}
    }

    sem_wait(sem_queue_empty);
    strncpy(shared_stuff->buffer[shared_stuff->line_write], key_line, LINE_SIZE);
    shared_stuff->line_write = (shared_stuff->line_write + 1) % NUM_LINE;
    sem_post(sem_queue_full);

    //释放信号量，删除内存区域
    if (shmdt(shared_memory) < 0) 
	{
        perror ( "shmdt");
		exit(1);
    }
}
