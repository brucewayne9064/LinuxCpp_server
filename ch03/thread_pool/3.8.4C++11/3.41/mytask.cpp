#include "mytask.h"
#include<stdio.h>

bool CMyTask::task_Init(int arg) //模拟任务，打印循环后的参数c和传入的参数arg
{
	long long i=0,c=0;
	while(c<10000000)
	{
		while(i<1000000000)
			i++;
		c++;
	}
	printf("%d---------%lld--------\n",arg,c);
}
