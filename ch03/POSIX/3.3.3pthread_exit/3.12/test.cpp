#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define PTHREAD_NUM    2

void *thrfunc1(void *arg)  //第一个线程函数，通过pthread_exit结束
{
	static int count = 10000;  //线程退出返回给主线程的值，static
	pthread_exit((void*)(&count));
}
void *thrfunc2(void *arg)  //第二个线程函数，通过return结束
{
	static int count = 222;
	return (void *)(&count);  //return返回void * 指针
}

int main(int argc, char *argv[])
{
	pthread_t pid[PTHREAD_NUM];
	int retPid;
	int *pRet1; //指针
	int * pRet2;


	if ((retPid = pthread_create(&pid[0], NULL, thrfunc1, NULL)) != 0)
	{
		perror("create pid first failed");
		return -1;
	}
	if ((retPid = pthread_create(&pid[1], NULL, thrfunc2, NULL)) != 0)
	{
		perror("create pid second failed");
		return -1;
	}  //创建两个线程

	if (pid[0] != 0)
	{
		pthread_join(pid[0], (void**)&pRet1);  //1.等待进程pid[0]结束  2.获取线程结束的返回值（二级指针，地址的地址）
		printf("get thread 0 exitcode: %d\n", *pRet1);
	}
	if (pid[1] != 0)
	{
		pthread_join(pid[1], (void**)&pRet2);
		printf("get thread 1 exitcode: %d\n", *pRet2);
	}
	return 0;
}