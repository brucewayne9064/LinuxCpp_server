#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
 
int a = 200; //货物
int b = 100;  //钱
pthread_mutex_t mutex;
 
void* ThreadA(void*)  //写操作
{
	while (1)
	{
        pthread_mutex_lock(&mutex);
		a -= 50; //卖出价值50元的货物 
		b += 50;//收回50元钱
        pthread_mutex_unlock(&mutex);
	}
}
 
void* ThreadB(void*)  //读操作，每隔1s查看钱和货的总值
{
	while (1)
	{
        
		printf("%d\n", a + b);
		sleep(1);    
	}
}
 
int main(int argc, char *argv[])
{
	pthread_t tida, tidb;

    pthread_mutex_init(&mutex, NULL);
 
	pthread_create(&tida, NULL, ThreadA, NULL);
	pthread_create(&tidb, NULL, ThreadB, NULL);
	pthread_join(tida, NULL);
	pthread_join(tidb, NULL);

    pthread_mutex_destroy(&mutex);
	return 0;
}