#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <semaphore.h>
#include "memory.h"

#define MAXREADER 1
#define NUMREADER 5
#define NUM 30
#define block_size 1024
//int为4bytes，每个目录项为64bytes，盘块1KB，则最多可以有(1024-4)/64=15个目录项
#define DIRTABLE_MAX_SIZE 15        //目录表项最大值

//目录项结构：
typedef struct dirUnit  //
{
    char name[NUM];                 //文件名
    char type;                      //文件类型,0为目录,1为文件
    int start;                      //起始盘块
}diru;

//目录表结构：
typedef struct dirTable 
{
    int diruAmount;                 //目录项数目
    diru dirs[DIRTABLE_MAX_SIZE];   //目录项列表
}dirt;

//文件控制块结构：
typedef struct FCB   
{
    int filesize;                   //文件大小，盘块为单位
    int datasize;                   //已写入的内容大小，字节为单位
    int block_index;                //文件数据起始盘块索引号
    int read_pointer;               //读指针，字节为单位
    int link;                       //文件链接数
    sem_t *write_semaphore;
}fcb;

dirt* root;                         //根目录
dirt* current;                      //当前位置
char path[200];                     //保存当前绝对路径

void init()
{
    //分配一个盘块空间给root
    int start = getBlock(1);
    if(start == -1)
    {
        return;
    }
    
    root = (dirt*)getBlockAddr(start);
    root->diruAmount = 0;

    current = root;

    path[0] = '/';
    path[1] = '\0';
}

//获得绝对路径
char* getPath()
{
    return path;
}

//展示当前目录 ls
void listDir()
{
    int unitAmount = current->diruAmount;
    printf("总计:%d\n", unitAmount);
    printf("文件名称\t文件类别\t文件大小\tFCB\t起始块\n");

    for(int i=0; i<unitAmount; i++)
    {
        //获取目录项
        diru unitTemp = current->dirs[i];
        printf("%8s\t", unitTemp.name);
        if(unitTemp.type == 0)
        {
            char s1[100] = "文件夹";
            printf("%10s\t", s1);
        }
        else if (unitTemp.type == 1)
        {
            char s1[100] = "文件";
            printf("%8s\t", s1);
        }
        //该表项是文件，继续输出大小和起始盘块号
        if(unitTemp.type == 1)
        {
            int fcbBlock = unitTemp.start;
            fcb* fileFCB = (fcb*)getBlockAddr(fcbBlock);
            char s1[100] = {0};
            char s2[100] = {0};
            char s3[100] = {0};
            sprintf(s1, "%d", fileFCB->filesize); 
            sprintf(s2, "%d", fcbBlock); 
            sprintf(s3, "%d", fileFCB->block_index); 
            
            printf("%12s\t%11s\t%s\n", s1,s2,s3);
        }
        else
        {
            int dirBlock = unitTemp.start;
            dirt* myTable = (dirt*)getBlockAddr(dirBlock);
            char s1[100] = {0};
            char s2[100] = {0};
           
            sprintf(s1, "%d", myTable->diruAmount); 
            sprintf(s2, "%d", unitTemp.start); 

            printf("%11s\t%11s\n",s1,s2);
        }
    }
}

//切换目录 cd
int cdDir(char dirName[])
{
    //目录项在目录位置
    int unitIndex = findUnitInTable(current, dirName);
    //不存在
    if(unitIndex == -1 && strcmp(dirName, "..") != 0)
    {
        printf("没有该目录！请检查...\n");
        return -1;
    }
    if(current->dirs[unitIndex].type == 1)
    {
        printf("这不是一个目录！请检查...\n");
        return -1;
    }
    //修改当前目录
    int dirBlock = current->dirs[unitIndex].start;
    current = (dirt*)getBlockAddr(dirBlock);
    //修改全局绝对路径
    if(strcmp(dirName, "..") == 0)
    {
        //回退绝对路径
        int len = strlen(path);
        for(int i=len-2;i>=0;i--)
        {
            if(path[i] == '/')
            {
                path[i+1]='\0';
                break;
            }
        }
    }
    else 
    {
        strcat(path, dirName);
        strcat(path, "/");
    }
    return 0;
}

//修改文件名或者目录名 mv
int mv(char oldName[], char newName[])
{
    int unitIndex = findUnitInTable(current, oldName);
    if(unitIndex == -1)
    {
        printf("文件未找到！\n");
        return -1;
    }
    strcpy(current->dirs[unitIndex].name, newName);
    return 0;
}

////////////////////////////////////////////////////////////////////////
//创建和删除文件，首先建立文件控制块

//创建FCB
int createFCB(int fcbBlockNum, int fileBlockNum, int filesize)
{
    //找到fcb的存储位置
    fcb* currentFCB = (fcb*) getBlockAddr(fcbBlockNum);
    currentFCB->block_index = fileBlockNum;//文件数据起始位置
    currentFCB->filesize = filesize;//文件大小
    currentFCB->link = 1;//文件链接数
    currentFCB->datasize = 0;//文件已写入数据长度
    currentFCB->read_pointer = 0;//文件读指针

    currentFCB->write_semaphore = sem_open("write_semaphore", O_CREAT, 0644, 1);
    if(currentFCB->write_semaphore == SEM_FAILED)
    {
        perror("sem_open error");
        exit(1);
    }
    return 0;
}

//创建文件 touch
int touch(char name[], int filesize)
{
    //检测文件名字长度
    if(strlen(name) >= NUM)
    {
        printf("文件名字过长！\n");
        return -1;
    }
    //获得FCB的空间
    int fcbBlock = getBlock(1);
    if(fcbBlock == -1)
        return -1;
    //获取文件数据空间
    int FileBlock = getBlock(filesize);
    if(FileBlock == -1)
        return -1;
    //创建FCB
    if(createFCB(fcbBlock, FileBlock, filesize) == -1)
        return -1;
    //添加到目录项
    if(add(current, name, 1, fcbBlock) == -1)
        return -1;

    return 0;
}

//创建目录 mkdir
int mkDir(char dirName[])
{
    if(strlen(dirName) >= NUM)
    {
        printf("文件夹名字过长！\n");
        return -1;
    }
    //为目录表分配空间
    int dirBlock = getBlock(1);
    if(dirBlock == -1)
        return -1;
    //将目录作为目录项添加到当前目录
    if(add(current, dirName, 0, dirBlock) == -1)
        return -1;
    
    //为新建的目录添加一个到父目录的目录项
    dirt *newTable = (dirt*)getBlockAddr(dirBlock);
    newTable->diruAmount = 0;
    char parent[] = "..";
    int ind = ((char*)current - startaddr)/block_size;
    if(add(newTable, parent, 0, ind) == -1)
        return -1;

    return 0;
}
//添加目录项
int add(dirt* myDirTable, char name[], int type, int FCBBlockNum)
{
    //获得目录表
    int diruAmount = myDirTable->diruAmount;
    //检测目录表示是否已满
    if(diruAmount == DIRTABLE_MAX_SIZE)
    {
        printf("内存已满！\n");
        return -1;
    }

    //是否存在同名文件
    if(findUnitInTable(myDirTable, name) != -1)
    {
        printf("文件已经存在！\n");
           return -1;
    }
    //构建新目录项
    diru* newdiru = &myDirTable->dirs[diruAmount];
    myDirTable->diruAmount++;//当前目录表的目录项数量+1
    //设置新目录项内容
    strcpy(newdiru->name, name);
    newdiru->type = type;
    newdiru->start = FCBBlockNum;

    return 0;
}

//删除文件 rm
int rm(char name[])
{
    //忽略系统的自动创建的父目录
    if(strcmp(name, "..") == 0)
    {
        printf("无法删除上层文件！\n");
        return -1;
    }
    //查找文件的目录项位置
    int unitIndex = findUnitInTable(current, name);
    if(unitIndex == -1)
    {
        printf("没有该文件！请检查...\n");
        return -1;
    }
    diru myUnit = current->dirs[unitIndex];
    //判断类型
    if(myUnit.type == 0)//目录
    {
        printf("这不是一个文件！请检查！\n");
        return -1;
    }
    int fcbBlock = myUnit.start;
    //释放内存
    Free(fcbBlock);
    //从目录表中剔除
    deleteDirt(current, unitIndex);
    return 0;
}

//释放文件内存
int Free(int fcbBlock)
{
    fcb* myFCB = (fcb*)getBlockAddr(fcbBlock);
    myFCB->link--;  //链接数减一
    //无链接，删除文件
    if(myFCB->link == 0)
    {
        //释放文件的数据空间
        releaseBlock(myFCB->block_index, myFCB->filesize);
    }
    //释放FCB的空间
    sem_close(myFCB->write_semaphore);
    sem_unlink("write_semaphore");
    releaseBlock(fcbBlock, 1);
    return 0;
}

//删除目录项
int deleteDirt(dirt* myDirTable, int unitIndex)
{
    //迁移覆盖
    int diruAmount = myDirTable->diruAmount;
    for(int i=unitIndex; i<diruAmount-1; i++)
    {
        myDirTable->dirs[i] = myDirTable->dirs[i+1];
    }
    myDirTable->diruAmount--;
    return 0;
}


//删除目录 rmdir
int rmDir(char dirName[])
{
    //忽略系统的自动创建的父目录
    if(strcmp(dirName, "..") == 0)
    {
        printf("无法删除上层目录！\n");
        return -1;
    }
    //查找文件
    int unitIndex = findUnitInTable(current, dirName);
    if(unitIndex == -1)
    {
        printf("没有该目录！请检查...\n");
        return -1;
    }
    diru myUnit = current->dirs[unitIndex];
    //判断类型
    if(myUnit.type == 0)//目录
    {
        deleteFileInTable(current, unitIndex);
    }
    else 
    {
        printf("这不是一个目录！请检查...\n");
        return -1;
    }
    //从目录表中剔除
    deleteDirt(current, unitIndex);
    return 0;
}

//删除文件/目录项
int deleteFileInTable(dirt* myDirTable, int unitIndex)
{
   //查找文件
    diru myUnit = myDirTable->dirs[unitIndex];
    //判断类型
    if(myUnit.type == 0)//目录
    {
        //找到目录位置
        int fcbBlock = myUnit.start;
        dirt* table = (dirt*)getBlockAddr(fcbBlock);
        //递归删除目录下的所有文件
        printf("已删除%s\n", myUnit.name);
        int unitCount = table->diruAmount;
        for(int i=1; i<unitCount; i++)//忽略“..”
        {
            printf("删除%s...\n", table->dirs[i].name);
            deleteFileInTable(table, i);
        }
        //释放目录表空间
        releaseBlock(fcbBlock, 1);
    }
    else 
    {
        int fcbBlock = myUnit.start;
        Free(fcbBlock);
    }
    return 0;
}

//从目录中查找目录项目
int findUnitInTable(dirt* myDirTable, char unitName[])
{
    //获得目录表
    int diruAmount = myDirTable->diruAmount;
    int unitIndex = -1;
    for(int i = 0; i < diruAmount; i++)
    {
        if(strcmp(unitName, myDirTable->dirs[i].name) == 0)
        {
            unitIndex = i;
        }
    }
    return unitIndex;
}

//读文件
int Read(char name[], int length)
{
    int unitIndex = findUnitInTable(current, name);
    if(unitIndex == -1)
    {
        printf("没有找到该文件！请检查...\n");
        return -1;
    }
    //控制块
    int fcbBlock = current->dirs[unitIndex].start;
    fcb* myFCB = (fcb*)getBlockAddr(fcbBlock);
    myFCB->read_pointer = 0; //文件指针重置
    //读数据
    char* data = (char*)getBlockAddr(myFCB->block_index);

    int datasize = myFCB->datasize;
    printf("读取结果为(以#为终止符):");
    for(int i = 0; i < length && myFCB->read_pointer < datasize; i++, myFCB->read_pointer++)
    {
        printf("%c", *(data+myFCB->read_pointer));
    }
    if(myFCB->read_pointer == datasize)//读到文件末尾用#表示
    {
        printf("#");
    }
    printf("\n请按任意键退出...\n");
    getchar();
   

    printf("\n");
    return 0;
}

//写文件，从末尾写入 write
int gedit(char name[], char content[])
{
    int unitIndex = findUnitInTable(current, name);
    if(unitIndex == -1)
    {
        printf("没有找到该文件！请检查...\n");
        return -1;
    }
    //控制块
    int fcbBlock = current->dirs[unitIndex].start;
    fcb* myFCB = (fcb*)getBlockAddr(fcbBlock);
   
    int contentLen = strlen(content);
    int filesize = myFCB->filesize * block_size;
    char* data = (char*)getBlockAddr(myFCB->block_index);

    //获得写者锁
    myFCB->write_semaphore = sem_open("write_semaphore", 0);

    //上写者锁
    sem_wait(myFCB->write_semaphore);

    for(int i = 0; i < contentLen && myFCB->datasize < filesize; i++, myFCB->datasize++)
    {
        *(data+myFCB->datasize) = content[i];
    }
    /* 模拟编辑器,控制写者不立即退出 */
    printf("请按任意键退出...\n");
    getchar();

    //释放写者锁
    sem_post(myFCB->write_semaphore);
    if(myFCB->datasize == filesize)
    {
        printf("无法继续写入！\n");
    }
    return 0;
}
