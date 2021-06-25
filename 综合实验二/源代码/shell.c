#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <pwd.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "func.h"


void help ()
{
    printf("*******************************欢迎进入文件系统！************************************\n");
    printf("以下指令可以帮助您使用该文件系统！\n");
    printf("open + <string> + <length>       新建一个文件,文件名为<string>,长度为length\n");
    printf("rm + <string>                    删除一个文件,文件名为<string>\n");
    printf("mv + <string1> + <string2>       重命名一个文件/文件夹<string1>,新文件名为<string2>\n");
    printf("mkdir + <string>                 新建一个文件夹,文件名为<string>\n");
    printf("rmdir + <string>                 删除一个文件夹,文件名为<string>\n");
    printf("gedit + <string> + <content>     编辑一个文件,文件名为<string>,内容为content\n");
    printf("read + <string> + <length>       读取一个文件,文件名为<string>,长度为length\n");
    printf("ls + <string>                    列举该文件夹下的所有文件\n");
    printf("cd + <string>                    打开下一个目录<string>\n");
    printf("cd ..                            返回上一个目录\n");
    printf("quit                             退出文件系统\n");
    printf("**********************************祝您使用愉快！*************************************\n");
}

/* 获得当前目录名 */
void GetDir()
{
    char *path = getPath();
    printf("%s", path);
}

int main()
{
    help();
    initsystem(); //初始化位示图
    init();       //初始化路径
	
    while(1)
    {
        GetDir();
        fflush(stdout);
        //读取字符串
        char str[1024];
        int s = read(0, str, 1024);
        if(s > 0)//读取到字符
        {
            int i = 0;
            for(i=0; i<s; ++i)
            {
                if(str[i] == '\b' && i >= 1)
                {
                //    printf("debug:%d\n",i);
                    int j = 0;
                    for(j=i+1; j<s; ++j)
                    {
                        str[j-2] = str[j];
                    }
                    str[s-2] = 0;
                    i -= 1;
                }
                else if(str[i] == '\b' && i == 0)
                {
                    int j = 0;
                    for(j=1; j<s; ++j)
                    {
                        str[j-1] = str[j];
                    }
                    str[s-1] = 0;
                }
                else
                {
                    continue;
                }
            }
            str[s] = 0;
        }
        else
        {
            continue;
        }
        //将读取到的字符串分成多个字符串
        char *start = str;
        int i = 1;
        char *Command[10] = {0};
        Command[0] = start;
        /* 跳过空格 */
        while(*start)
        {
            if(isspace(*start))
            {
                *start = 0;
                start++;
                Command[i++] = start;
            }
            else
            {
                ++start;
            }
        }
        Command[i-1] = NULL;

        if(!strcmp(Command[0], "ls"))
        {
            listDir();
        }
        else if(!strcmp(Command[0], "cd"))
        {
            if(Command[1] != NULL)
            {
                cdDir(Command[1]);
            }
            else
            {
                printf("操作失败！\n");
            }
        }
        else if(!strcmp(Command[0], "mv"))
        {
            if(Command[1] != NULL && Command[2] != NULL)
            {
                mv(Command[1], Command[2]);
            }
            else
            {
                printf("操作失败！\n");
            }
        }
        else if(!strcmp(Command[0], "open"))
        {
            if(Command[1] != NULL && Command[2] != NULL)
            {
                touch(Command[1], atoi(Command[2]));
            }
            else
            {
                printf("操作失败！\n");
            }
        }
        else if(!strcmp(Command[0], "mkdir"))
        {
            if(Command[1] != NULL)
            {
                mkDir(Command[1]);
            }
            else
            {
                printf("操作失败！\n");
            }
        }
        else if(!strcmp(Command[0], "rm"))
        {
            if(Command[1] != NULL)
            {
                rm(Command[1]);
            }
            else
            {
                printf("操作失败！\n");
            }
        }
        else if(!strcmp(Command[0], "rmdir"))
        {
            if(Command[1] != NULL)
            {
                rmDir(Command[1]);
            }
            else
            {
                printf("操作失败！\n");
            }
        }
        else if(!strcmp(Command[0], "read"))
        {
            if(Command[1] != NULL && Command[2] != NULL)
            {
                Read(Command[1], atoi(Command[2]));
            }
            else
            {
                printf("操作失败！\n");
            }
        }
        else if(!strcmp(Command[0], "gedit"))
        {
            if(Command[1] != NULL && Command[2] != NULL)
            {
                gedit(Command[1], Command[2]);
            }
            else
            {
                printf("操作失败！\n");
            }
        }
        else if(!strcmp(Command[0], "quit"))
        {
            exitsystem();
            exit(0);
        }
        else
        {
            printf("没有这个指令，请检查！\n");
        }
    }
    return 0;
}