#include<pthread.h>
#include<iostream>

void *thfunc(void *arg)
{
    char *str;
    str = (char *)arg;
    std::cout << "in thread: str = " << str << std::endl;
    return (void *)0;
}

int main(int argc, char *argv[])
{
    pthread_t tidp;
    int ret;
    const char * str = "hello world";
    ret = pthread_create(&tidp, NULL, thfunc, (void *)str);
	if (ret)
	{
		printf("pthread_create failed:%d\n", ret);
		return -1;
	}
    pthread_join(tidp, NULL);  //等待子进程结束
	printf("in main:thread is created\n");   
    return 0;
}