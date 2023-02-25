#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h> //strerror
 
void mycleanfunc(void *arg)  //清理函数
{
	printf("mycleanfunc:%d\n", *((int *)arg));//打印传入的参数						 
}
void *thfrunc1(void *arg)
{
	int m=1;
	printf("thfrunc1 comes \n");
	pthread_cleanup_push(mycleanfunc, &m); //清理函数压栈，m是传入的参数
	return (void *)0;	 //线程退出
	pthread_cleanup_pop(0);	//清理函数出栈（不执行）
}
 
void *thfrunc2(void *arg)
{
	int m = 2;
	printf("thfrunc2 comes \n");
	pthread_cleanup_push(mycleanfunc, &m); //清理函数压栈，m是传入的参数
	pthread_exit(0);  //线程退出
	pthread_cleanup_pop(0);	//清理函数出栈（不执行）
}


int main(void)
{
	pthread_t pid1,pid2;
	int res;
	res = pthread_create(&pid1, NULL, thfrunc1, NULL);  //创建线程1
	if (res) 
	{
		printf("pthread_create failed: %d\n", strerror(res));  //char *strerror(int errnum) 从内部数组中搜索错误号errnum，并返回一个指向错误消息字符串的指针
		exit(1);
	}
	pthread_join(pid1, NULL);  //等待线程1结束
	
	res = pthread_create(&pid2, NULL, thfrunc2, NULL);  //创建线程2
	if (res) 
	{
		printf("pthread_create failed: %d\n", strerror(res));
		exit(1);
	}
	pthread_join(pid2, NULL);  //等待线程2结束
	
	printf("main over\n");
	return 0;
}
