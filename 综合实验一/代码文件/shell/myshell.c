#include <stdio.h>
#include <string.h>
#include <pwd.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>  
#include <sys/wait.h>

void prompt_info();
void printHelp();
int execmd(char* argv[]);
void command_analyzed();

int main()
{
    while(1)
    {
        prompt_info();
        command_analyzed();
    }
    return 0; 
}

void prompt_info()
{
    char prompt[256];
    struct passwd *pwd;
	
	//获取用户识别码,用来获得username
    pwd = getpwuid(getuid());

    char username[256], hostname[256], cwd[256];
	
	//获得username
    strcpy(username, pwd->pw_name);
	
	//获得主机名
    gethostname(hostname, sizeof(hostname));

    int i = 0, j = 0;
    
	//ruichen@ruichen-virtual-machine:~/桌面/oslab/com-exp1$ 
	//[--myshell--][ruichen@ruichen-virtual-machine com-exp1]$
    //获得文件绝对路径 (eg. /home/ruichen/桌面/oslab/com-exp1)
    getcwd(cwd, sizeof(cwd));
	char *p[256];
	p[0]= strtok(cwd, "/");
	i = 0;
	while (p[i] != NULL)
	{
    	i++;
    	p[i]= strtok(NULL, "/");
	}

	//拼接字符串
	strcpy(prompt, "[MyShell]["); 
    strcat(prompt, username);
    strcat(prompt, "@");
    strcat(prompt, hostname);
	strcat(prompt, " ");
	for (int j = 0; j < i; j++)
	{
		strcat(prompt, p[j]);
		if (j != i - 1)
		{
			strcat(prompt, "/");
		}
	}  
	strcat(prompt, "]#");
	/*
	字体色
	30      Black
	31      Red
	32      Green
	33      Yellow
	34      Blue
	35      Magenta
	36      Cyan
	37      White*/
    printf("\e[1;31;40m %s \e[0m", prompt);
}

void printHelp()
{
	FILE *fp;
    fp = 0;
    //切换模式，改成只读
    if((fp = fopen("./help.txt","r")) == 0)
	{
        printf("文件打开失败!\n");
        return -1;
    }
    char strBuf[1000];
    memset(strBuf,0,sizeof(strBuf));
    while(1)
    {
        if(fgets(strBuf,sizeof(strBuf),fp) == 0)
		{
			break;
		}
        printf("%s",strBuf);
    }
    fclose(fp);
return;
}

int execmd(char* argv[])
{
    pid_t pid = fork();
    if(pid == 0)
    {
        if(argv[0])
		{
            if(execvp(argv[0], argv) == -1)
			{
                printf("invalid command!\n");
			}
		}
        exit(-1);
    }
    else if(pid>0)
    {
        wait(NULL);
    }
    else
    {
        perror("error");
        return 0;
    }
    return 1;
}

void command_analyzed()
{
    char reader[256], c, *reader1[256], *reader2[256];
    int i = 0, j = 0, p = -1;
    for(i = 0; i < 8; i++)
    {
        reader1[i] = (char *)malloc(256*sizeof(char));
        reader2[i] = (char *)malloc(256*sizeof(char));
    }
        
    i = 0;
    j = 0;
	//分割字符串
    while((c=getchar())!='\n')
    {      
        if(c == '|')  //读到重定向符号
		{
            p = i;
			i++;
		}
        else if(c != ' ')  //正常非空格字符，正常读取
		{
            reader[j] = c;   
			j++;
		}
        else  //空格字符
        {
            reader[j] = '\0';
            strcpy(reader1[i], reader);
            strcpy(reader2[i], reader1[i]);
            reader2[i][j] = 0;
            i++;
            j = 0;
        }
    }
    if(i == 0 && j == 0)   //什么都没有输入
	{
         return;
	}
    reader[j] = '\0';
    strcpy(reader1[i], reader);
    strcpy(reader2[i], reader1[i]);
    reader2[i][j] = 0;
    i++;
    reader2[i] = 0;
    
    if(p == -1) // no pipe
    {
        if(!strcmp("exit", reader1[0]))
        {
            printf("Bye from myshell.\n");
            exit(EXIT_SUCCESS);
        }
		else if(!strcmp("echo", reader1[0]))
        {
            printf("%s\n", reader1[1]);
        }
        else if(!strcmp("cd", reader1[0]))
        {
            if(chdir(reader1[1]) < 0)
			{
                printf("No such path or directory! Please check your spelling.\n");
			}
        }
		else if(!strcmp("help", reader1[0]))
        {
            printHelp();
        }
        else
        {
            execmd(reader2);
        }
    }
    else // pipe
    {
        pid_t pid1 = 0;
        int fds[2];
        char *des1[256], *des2[256];
        pipe(fds);  //创建管道，fds存储文件描述符
        
        for(j=0; j<p; j++)
        {
            des1[j] = reader2[j]; // cmd1
        }
        des1[p] = 0; 

        for(j=p; j<i; j++)
        {
            des2[j] = reader2[j]; // cmd2
        }     
     
        pid1 = fork();
        if(pid1 < 0)
		{
            printf("Fork error!\n");
		}
        else if(pid1 == 0)
        {
            close(fds[0]); 
            dup2(fds[1], stdout);
            execvp(des1[0], des1);
            close(fds[1]);
        }
        else
        {           
            waitpid(pid1,NULL,0);
            pid_t pid2 = fork();
            if(pid2 < 0)
			{
                printf("Fork error!\n");
			}
            else if(pid2 == 0)
            {
                close(fds[1]);  
                dup2(fds[0], stdin);
                execvp(des2[0], des2);
                close(fds[0]);
            }
            else
            {
                close(fds);
                wait(pid2);
            } 
        }
    }
    return;
}