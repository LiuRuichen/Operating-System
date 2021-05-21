#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <time.h>
#define getpch(type) (type*) malloc(sizeof(type))
typedef struct pcb PCB;
struct pcb  //定义进程控制块PCB
{
	int id;         //标识符
	char name[10];  //名称
	int time_start; //到达时间
	int time_need;  //服务时间
	int time_left;  //剩余运行时间
	int time_used;  //已使用的CPU时间
	//time_need = time_left + time_used
	char state;     //进程状态
};
void _sleep (int n)  //停顿n秒
{
	clock_t goal;
	goal = (clock_t)n * 1000 + clock();
	while(goal > clock());
}
char _keygo()        //程序暂停，按任意键继续
{
	char c;
	printf("按任意键继续......\n");
	c = getch();
	return c;
}

//------------数据设置区域-----------------
int time_unit = 2;  //时间片长度
const int maxnum = 10;  //最大进程数量
int num = 5;        //实际进程数量
PCB pcbdata[maxnum] = 
{
	{1000,"A",0,4,4,0,'R'},
	{1001,"B",1,3,3,0,'R'},
	{1002,"C",2,5,5,0,'R'},
	{1003,"D",3,2,2,0,'R'},
	{1004,"E",4,4,4,0,'R'},
};
int ready[maxnum];    //就绪队列，存放进程在pcbdata中的位置

int order[maxnum];

void input()
{
	int i;
	printf("进程总数为：");
	scanf("%d",&num);
	for(i=0; i<num; i++)
	{
		pcbdata[i].id = 1000 + i;
		printf("输入第%d个进程名：",i+1);
		scanf("%s",&pcbdata[i].name);
		printf("输入第%d个进程到达时间：",i+1);
		scanf("%d",&pcbdata[i].time_start);
		printf("输入第%d个进程服务时间：",i+1);
		scanf("%d",&pcbdata[i].time_need);
		pcbdata[i].time_left = pcbdata[i].time_need; //剩余运行时间
		printf("\n");
		pcbdata[i].time_used = 0;
		pcbdata[i].state = 'R';
	}
}

//------------数据设置区域-----------------
void FCFS()  //先来先服务算法
{
	int i, j, temp;
	double k;
	for(i=0; i<num; i++)
	{
		order[i] = pcbdata[i].time_start;
		ready[i] = i;
	}
	for(i=0; i<num; i++)
	{
		for(j=i+1; j<num; j++)
		{
			if(order[i] > order[j])
			{
				temp = order[i];
				order[i] = order[j];
				order[j] = temp;

				temp = ready[i];
				ready[i] = ready[j];
				ready[j] = temp;
			}
		}
	}
	printf("---先来先服务算法调度：非抢占，无时间片---\n");
	temp = pcbdata[ready[0]].time_start;
	for(i=0; i<num; i++)
	{
		printf("第%d个进程--%s, ",i+1,pcbdata[ready[i]].name);
		printf("到达时间--%d, 服务时间--%d\n",pcbdata[ready[i]].time_start,pcbdata[ready[i]].time_need);
		printf("本进程正在运行......\n");
		_sleep(1);
		printf("运行完毕\n");
		temp += pcbdata[ready[i]].time_need;
		j = temp - pcbdata[ready[i]].time_start;
		k = (float)j / pcbdata[ready[i]].time_need;
		printf("完成时间--%d, 周转时间--%d, 带权周转时间--%.1f\n",temp,j,k);
	}

	printf("------所有进程调度完毕-------\n");
}

void SJF ()
{
	int N = 0;
	int i, record, temp, j, tmp, time_stay;
	float time_stay_w;
	int f = 99;
	int ff = 99;
	int visited[num];

	for(i=0; i<num; i++)
	{
		visited[i] = 0;
	}

	for(i=0; i<num; i++)
	{
		order[i] = pcbdata[i].time_start;
	}

	for(i=0; i<num; i++)
	{
		if(f >= order[i] && ff > pcbdata[i].time_need)
		{
			f = order[i];
			ff = pcbdata[i].time_need;
			ready[0] = i;
		}
	}

	N++;
	temp = order[ready[0]];
	printf("---短作业进程优先算法调度：非抢占，无时间片---\n");
	int ct = 1;
	while(N)
	{
		visited[ready[0]] = 1;
		printf("第%d个进程--%s, ",ct,pcbdata[ready[0]].name);
		ct++;
		printf("到达时间--%d, 服务时间--%d\n",order[ready[0]],pcbdata[ready[0]].time_need);
		printf("本进程正在运行......\n");
		pcbdata[ready[0]].state = 'E';
		_sleep(1);
		printf("运行完毕\n");
		pcbdata[ready[0]].state = 'F';
		temp += pcbdata[ready[0]].time_need;
		time_stay = temp - pcbdata[ready[0]].time_start;
		time_stay_w = (float)time_stay / pcbdata[ready[0]].time_need;
		printf("完成时间--%d, 周转时间--%d, 带权周转时间--%.1f\n",temp,time_stay,time_stay_w);
		N = 0;  //重置ready数组
		for(i=0; i<num; i++)  
		{
			if(pcbdata[i].time_start < temp && visited[i] == 0)  //当前已存在的所有进程且没有执行过
			{
				ready[N] = i;
				N++;
			}
		}

		for(i=0; i<N; i++)   //对ready数组进行排序，得到最短作业(bubble sort)
		{
			for(j=i+1; j<N; j++)
			{
				if(pcbdata[ready[i]].time_need > pcbdata[ready[j]].time_need)
				{
					tmp = ready[i];
					ready[i] = ready[j];
					ready[j] = tmp;
				}
			}
		}	
	}	
	printf("------所有进程调度完毕-------\n");
}

void HRF ()
{
	float HRF[num+1]; //保存响应比
	int N = 0;
	int i, record, temp, j, tmp, time_stay;
	float time_stay_w;
	int f = 99;
	int ff = 99;
	int visited[num];

	for(i=0; i<num; i++)
	{
		visited[i] = 0;
	}

	for(i=0; i<num; i++)
	{
		order[i] = pcbdata[i].time_start;
	}

	for(i=0; i<num; i++)
	{
		if(f > order[i])
		{
			f = order[i];
			ready[0] = i;
		}
	}

	N++;
	temp = order[ready[0]];
	printf("---高响应比进程优先算法调度：非抢占，无时间片---\n");
	int ct = 1;
	while(N)
	{
		visited[ready[0]] = 1;
		printf("第%d个进程--%s, ",ct,pcbdata[ready[0]].name);
		ct++;
		printf("到达时间--%d, 服务时间--%d\n",order[ready[0]],pcbdata[ready[0]].time_need);
		printf("本进程正在运行......\n");
		pcbdata[ready[0]].state = 'E';
		_sleep(1);
		printf("运行完毕\n");
		pcbdata[ready[0]].state = 'F';
		temp += pcbdata[ready[0]].time_need;
		time_stay = temp - pcbdata[ready[0]].time_start;
		time_stay_w = (float)time_stay / pcbdata[ready[0]].time_need;
		printf("完成时间--%d, 周转时间--%d, 带权周转时间--%.1f\n",temp,time_stay,time_stay_w);
		N = 0;
		char c = 0;
		for(i=0; i<num; i++)  //得到所有当前可以运行的进程编号
		{
			if(pcbdata[i].time_start < temp && visited[i] == 0)
			{
				ready[N] = i;
				N++;
			}
		}

		//计算响应比
		for(i=0; i<N; i++)
		{
			HRF[ready[i]] = (float)(temp - pcbdata[ready[i]].time_start + pcbdata[ready[i]].time_need) / pcbdata[ready[i]].time_need;
			printf("进程%s的响应比为%.1f\n",pcbdata[ready[i]].name,HRF[ready[i]]);
		}	

		float maxi = -1;
		int res;
		for(i=0; i<N; i++)
		{
			if (maxi < HRF[ready[i]])
			{
				maxi = HRF[ready[i]];
				res = ready[i];
			}
		}
		ready[0] = res;
	}	
	printf("------所有进程调度完毕-------\n");
}

//定义队列结构
typedef struct queue
{
    int *list = (int*) malloc(100*sizeof(int));
    int front;
    int rear;
}Queue;

void init (Queue *SQ)
{
    SQ->front = 0;
    SQ->rear = 0;
}

int front (Queue *SQ)
{
	return SQ->list[0];
}

int empty (Queue *SQ)
{
    if (SQ->front == SQ->rear && SQ->front == 0)
    {
        return 1;
    }
    else
    {
        return 0;
    } 
}

void push (Queue *SQ, int expr)
{
    SQ->list[SQ->rear] = expr;
    SQ->rear = SQ->rear + 1;
}

int pop (Queue *SQ)
{
    if (SQ->front == SQ->rear)
    {
        return -1;
    }
    else
    {
        int length = SQ->rear - SQ->front;
        for(int i = 0; i < length - 1; i++)
        {
            SQ->list[i] = SQ->list[i+1];
        }
        SQ->rear = SQ->rear - 1;
        return 0;
    }
    
}
void Timeslice ()  //time_unit = 2
{
	int i,j,rec,temp,cur;
	int visited[num+1];
	for(i=0; i<num+1; i++)
	{
		visited[i] = 0;
	}
	Queue SQ;  //定义队列
	init(&SQ); //队列初始化

	for(i=0; i<num; i++)
	{
		order[i] = pcbdata[i].time_start;
		ready[i] = i;
	}
	for(i=0; i<num; i++)
	{
		for(j=i+1; j<num; j++)
		{
			if(order[i] > order[j])
			{
				temp = order[i];
				order[i] = order[j];
				order[j] = temp;

				temp = ready[i];
				ready[i] = ready[j];
				ready[j] = temp;
			}
		}
	}

	int start = order[0];  //开始时间
	cur = start;

	push(&SQ,ready[0]);
	visited[ready[0]] = 1;

	int flag;
	while(!empty(&SQ))
	{
		flag = 0;
		pcbdata[front(&SQ)].state = 'E';
		pcbdata[front(&SQ)].time_left -= time_unit;
		if(pcbdata[front(&SQ)].time_left == 0)  
		{
			pcbdata[front(&SQ)].state = 'F';
			int cur1 = cur;
			cur += time_unit;
			printf("在时间片%d->%d区间发生的事情：\n",cur1,cur);
			printf("当前运行的进程：%s\n",pcbdata[front(&SQ)].name);
			printf("进程%d运行结束，其名称为%s，周转时间为%d，带权周转时间为%.1f，进程状态为%c",
			pcbdata[front(&SQ)].id,pcbdata[front(&SQ)].name,
			cur-pcbdata[front(&SQ)].time_start,
			(float)(cur-pcbdata[front(&SQ)].time_start)/pcbdata[front(&SQ)].time_need,
			pcbdata[front(&SQ)].state);
			if(pcbdata[front(&SQ)].state == 'E')
			{
				printf("（运行）\n");
			}
			else if(pcbdata[front(&SQ)].state == 'F')
			{
				printf("（结束）\n");
			}
		}
		else if (pcbdata[front(&SQ)].time_left < 0)
		{
			//eg.如果时间片为3，但是只用了一个，此时time_left为-2，而cur仅仅应该+1 
			pcbdata[front(&SQ)].state = 'F';
			int cur1 = cur;
			cur += (pcbdata[front(&SQ)].time_left + time_unit);  
			printf("在时间片%d->%d区间发生的事情：\n",cur1,cur);
			printf("当前运行的进程：%s\n",pcbdata[front(&SQ)].name);
			printf("进程%d运行结束，其名称为%s，周转时间为%d，带权周转时间为%.1f，进程状态为%c",
			pcbdata[front(&SQ)].id,pcbdata[front(&SQ)].name,
			cur-pcbdata[front(&SQ)].time_start,
			(float)(cur-pcbdata[front(&SQ)].time_start)/pcbdata[front(&SQ)].time_need,
			pcbdata[front(&SQ)].state);
			if(pcbdata[front(&SQ)].state == 'E')
			{
				printf("（运行）\n");
			}
			else if(pcbdata[front(&SQ)].state == 'F')
			{
				printf("（结束）\n");
			}
		}
		else  //该进程未运行完毕
		{
			int cur1 = cur;
			cur += time_unit;
			printf("在时间片%d->%d区间发生的事情：\n",cur1,cur);
			printf("当前运行的进程：%s\n",pcbdata[front(&SQ)].name);   
			printf("该时间片运行的是进程%d,名称为%s,该进程还需服务时间%d，进程状态为%c（运行）\n",
			pcbdata[front(&SQ)].id,
			pcbdata[front(&SQ)].name,
			pcbdata[front(&SQ)].time_left,
			pcbdata[front(&SQ)].state);    
			flag = 1;        
		}
		
		for(i=0; i<num; i++)
		{
			if(visited[i] == 0 && pcbdata[i].time_start <= cur)  //没有进入过队列且当前已经进来了的进程
			{
				push(&SQ,i);
				visited[i] = 1;
			}
		}

		if(flag == 1)  //该进程未运行完毕
		{
			printf("在该时间片的最后，队列发生的变化：");
			printf("[");
			int length = SQ.rear;
			for(j=0; j<length-1; j++)
			{
				printf("%s ",pcbdata[SQ.list[j]].name);
			}
			printf("%s]",pcbdata[SQ.list[j]].name);
			printf("--->");

			int f = front(&SQ);
			pop(&SQ);  //先弹出队首
			push(&SQ,f);  //再推到队尾
			printf("[");
			length = SQ.rear;
			for(j=0; j<length-1; j++)
			{
				printf("%s ",pcbdata[SQ.list[j]].name);
			}
			printf("%s]",pcbdata[SQ.list[j]].name);
			printf("\n");

		}

		else
		{
			printf("在该时间片的最后，队列发生的变化：");
			printf("[");
			int length = SQ.rear;
			for(j=0; j<length-1; j++)
			{
				printf("%s ",pcbdata[SQ.list[j]].name);
			}
			printf("%s]",pcbdata[SQ.list[j]].name);
			printf("--->");

			pop(&SQ);  //先弹出队首
			if(empty(&SQ))
			{
				printf("[]\n");
			}
			else
			{
				printf("[");
				length = SQ.rear;
				for(j=0; j<length-1; j++)
				{
					printf("%s ",pcbdata[SQ.list[j]].name);
				}
				printf("%s]",pcbdata[SQ.list[j]].name);
				printf("\n");
			}
		}

		printf("----------------------------------------------------------\n");
	}	
	printf("------所有进程调度完毕-------\n");
}

int main ()
{
	int i = 0, sch = 99;
	//input();
	
	while(sch != 0)
	{
		printf("\n请选择其中一种调度算法:\n");
		printf("(1)先来先服务算法FCFS\n");
		printf("(2)短作业优先算法SJF\n");
		printf("(3)高相应比算法HRF\n");
		printf("(4)时间片流转算法Timeslice\n");
		printf("(0)退出：\n");

		printf("请输入上述一个数字：");
		scanf("%d",&sch);

		switch(sch)
		{
			case 1:FCFS();break;
			case 2:SJF();break;
			case 3:HRF();break;
			case 4:Timeslice();break;
			case 0:printf("退出程序\n");;break;
		}
	}
	system("pause");
}