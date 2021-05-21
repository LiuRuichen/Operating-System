#include <fcntl.h> 
#include <sys/stat.h>
#include <semaphore.h> 

#define LINE_SIZE 256
#define NUM_LINE 16

const char *queue_mutex = "queue_mutex";
const char *queue_empty = "queue_empty";
const char *queue_full = "queue_full";

/*生产者消费者公用的缓冲区*/
struct shared_mem_st 
{
    char buffer[NUM_LINE][LINE_SIZE];  //缓冲区
    int line_write;  //读指针
    int line_read;   //写指针
};