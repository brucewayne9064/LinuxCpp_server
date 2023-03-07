#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h> 
#include <string.h>
#include <cstdlib>
 
int gcn = 0;

pthread_mutex_t mutex;  //定义一个互斥变量
	
void *thread_1(void *arg) {
	int j;
	for (j = 0; j < 10000000; j++) {
		pthread_mutex_lock(&mutex);  //加锁
		gcn++;
		pthread_mutex_unlock(&mutex);  //解锁
	}  
	pthread_exit((void *)0);
}

void *thread_2(void *arg) {
	int j;
	for (j = 0; j < 10000000; j++) {
		pthread_mutex_lock(&mutex);  
		gcn++;
		pthread_mutex_unlock(&mutex);    
	}  
	pthread_exit((void *)0);
}
int main(void) 
{
	int j,err;
	pthread_t th1, th2;
	 
	pthread_mutex_init(&mutex, NULL); //初始化互斥锁，属性为默认（NULL）
	for (j = 0; j < 10; j++)
	{
		err = pthread_create(&th1, NULL, thread_1, (void *)0);
		if (err != 0) {
			printf("create new thread error:%s\n", strerror(err));
			exit(0);
		}  
		err = pthread_create(&th2, NULL, thread_2, (void *)0);
		if (err != 0) {
			printf("create new thread error:%s\n", strerror(err));
			exit(0);
		}  
           
		err = pthread_join(th1, NULL);
		if (err != 0) {
			printf("wait thread done error:%s\n", strerror(err));
			exit(1);
		}
		err = pthread_join(th2, NULL);
		if (err != 0) {
			printf("wait thread done error:%s\n", strerror(err));
			exit(1);
		}
		printf("gcn=%d\n", gcn);  //每次打印都是20000000
		gcn = 0;  //gcn清零
	}
	pthread_mutex_destroy(&mutex); //销毁互斥锁

	return 0;
}