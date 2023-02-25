#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h> //strerror
 
void mycleanfunc(void *arg)
{
	printf("mycleanfunc:%d\n", *((int *)arg));	//打印传入的参数					 
}
void *thfrunc1(void *arg)
{
	int m=1,n=2;
	printf("thfrunc1 comes \n");
	pthread_cleanup_push(mycleanfunc, &m); 
	pthread_cleanup_push(mycleanfunc, &n); 
	pthread_cleanup_pop(1);  //出栈一个clean函数，并且执行 n = 2
	pthread_exit(0);  //退出线程,引发clean函数执行，  m = 1
	pthread_cleanup_pop(0);  //不执行
}
  
int main(void)
{
	pthread_t pid1 ;
	int res;
	res = pthread_create(&pid1, NULL, thfrunc1, NULL);
	if (res) 
	{
		printf("pthread_create failed: %d\n", strerror(res));
		exit(1);
	}
	pthread_join(pid1, NULL);
	
	printf("main over\n");
	return 0;
}
