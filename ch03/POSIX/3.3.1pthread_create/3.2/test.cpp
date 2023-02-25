#include<pthread.h>
#include<iostream>

void *thfunc(void *arg)
{
    int * p = (int *)arg;
    int n = *p;
    std::cout << "in thread: n = " << n << std::endl;
    return (void *)0;
}

int main(int argc, char *argv[])
{
    pthread_t tidp;
    int ret;
    int n = 110;
    ret = pthread_create(&tidp, NULL, thfunc, &n);
	if (ret)
	{
		printf("pthread_create failed:%d\n", ret);
		return -1;
	}
    pthread_join(tidp, NULL);  //等待子进程结束
	printf("in main:thread is created\n");   
    return 0;
}