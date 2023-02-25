#include <pthread.h>
#include <stdio.h>
#include <unistd.h> //sleep

void *thfunc(void *arg)  //线程函数，线程创建后要执行的函数
{
	printf("in thfunc\n");
	return (void *)0;  //返回一个空指针
}
int main(int argc, char *argv [])
{
	pthread_t tidp;  //pid
	int ret;

	ret = pthread_create(&tidp, NULL, thfunc, NULL);  //创建线程,成功返回0，失败返回错误代码
	if (ret != 0)
	{
		printf("pthread_create failed:%d\n", ret);
		return -1;
	}
	else{
	    sleep(1);  //main线程挂起1s，让子线程有机会执行
	    printf("in main:thread is created\n");
    }
	return 0;
}

