#include<stdio.h>
#include<stdlib.h>

#define system_size 100*1024*1024           //系统大小，100M
#define block_size 1024                     //盘块大小，1KB
#define block_count system_size/block_size  //系统盘块数目，100*1024

char *startaddr;  //起始地址

//系统初始化
void initsystem()
{
    /* 分配100M空间 */
    startaddr = (char*)malloc(system_size * sizeof(char));

    //初始化盘块的位示图
    for(int i = 0; i < block_count; i++)
    {
        startaddr[i] = '0';
    }

    //用于存放位示图bitmap的空间已被占用，计算占用盘块数为100
    int bitMapSize = block_count * sizeof(char) / block_size;

    //allocate from the start
    for(int i=0; i<bitMapSize; i++)
    {
        startaddr[i] = '1';   //盘块已被使用
    }
}

//磁盘分配 
int getBlock(int blockSize)
{
    int startBlock = 0;
    int sum = 0;
    for(int i = 0; i < block_count; i++)
    {
        if(startaddr[i] == '0')
        {
            if(sum == 0)
            {
                startBlock = i;
            }
            sum++;
            if(sum == blockSize)
            {
                for(int j = startBlock; j < startBlock+blockSize; j++)
                {
                    startaddr[j] = '1';
                }
                return startBlock;
            }
        }
        else
        {
            sum = 0;
        }
    }
    return -1;
}

//获得盘块的物理地址 
char* getBlockAddr(int blockNum)
{
    return startaddr + blockNum * block_size; //偏移量单位为字节
}

//释放盘块 
int releaseBlock(int blockNum, int blockSize)
{
    int endBlock = blockNum + blockSize;
    //修改位示图盘块的位置为0
    for(int i = blockNum; i < endBlock; i++)
    {
        startaddr[i] = '0';
    }
    return 0;
}

//退出系统 
void exitsystem()
{
    free(startaddr);
}

