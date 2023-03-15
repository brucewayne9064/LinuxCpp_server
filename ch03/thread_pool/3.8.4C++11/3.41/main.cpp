#include "XThreadPool.h"
#include "XThread.h"
#include "mytask.h"

#define XThreadPoolGet XThreadPool::Get()

int main() 
{
	int i;
 
	XThreadPoolGet->Init(10);//初始化线程池, 创建十个线程，放入线程池
	CMyTask task[10];   //创建十个任务
	for(i=0;i<10;i++)
		XThreadPoolGet->Dispatch(&task[i],i);//每次分配一个任务，传入task[i]的地址和编号
}