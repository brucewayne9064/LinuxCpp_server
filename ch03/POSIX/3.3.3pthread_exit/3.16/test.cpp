#include<stdio.h>  
#include<stdlib.h>  
#include <pthread.h>  
#include <unistd.h> //sleep
void *thfunc(void *arg)  
{  
	int i = 1;  
	printf("thread start-------- \n");  
	// while (1) 
    // {
    //     i++;
    //     if( i == 5)
    //     {
    //         pthread_testcancel();
            
    //     }
    // } //死循环
    while(1){
        printf("线程结束");
        sleep(1);
    }
    
	
	return (void *)0;  
}  
/* 取消线程失败的例子 */
int main()  
{  
	void *ret = NULL;  
	int iret = 0;  
	pthread_t tid;  
	pthread_create(&tid, NULL, thfunc, NULL);  
	sleep(1);  
          
	pthread_cancel(tid);//取消线程，如果成功返回0，否则返回错误代码
	pthread_join(tid, &ret);  //等待线程结束，获取线程结束的返回值
	if (ret == PTHREAD_CANCELED)
		printf("thread has stopped,and exit code: %p\n", ret);  
	else  //ret == PTHREAD_CANCELED == -1
		printf("some error occured");
          
	return 0;  
          
}  