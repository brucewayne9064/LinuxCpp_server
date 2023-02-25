#ifndef _GNU_SOURCE  
#define _GNU_SOURCE     /* To get pthread_getattr_np() declaration */  
#endif  
#include <pthread.h>  
#include <stdio.h>  
#include <stdlib.h>  
#include <unistd.h>  
#include <errno.h>  
      
#define handle_error_en(en, msg) \  
        do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)  
      

      
static void * thread_start(void *arg)  //子线程
{  
	int i,s;  
	pthread_attr_t gattr;  //线程属性union
 
      
	s = pthread_getattr_np(pthread_self(), &gattr);  //获得线程属性结构值，存储在gattr
	if (s)  
		handle_error_en(s, "pthread_getattr_np");  
      
	printf("Thread's detachstate attributes:\n");  
 
	s = pthread_attr_getdetachstate(&gattr, &i);  //设置线程分离状态属性
	if (s)  
		handle_error_en(s, "pthread_attr_getdetachstate");  
	printf("Detach state        = %s\n",
		(i == PTHREAD_CREATE_DETACHED) ? "PTHREAD_CREATE_DETACHED" :  
		(i == PTHREAD_CREATE_JOINABLE) ? "PTHREAD_CREATE_JOINABLE" :  
		"???");  

    pthread_detach(pthread_self());  //转换为可分离线程

    s = pthread_getattr_np(pthread_self(), &gattr);  //获得线程属性结构值，存储在gattr
	if (s)  
		handle_error_en(s, "pthread_getattr_np");  
      
	printf("Thread's detachstate attributes:\n");  
 
	s = pthread_attr_getdetachstate(&gattr, &i);  //设置线程分离状态属性
	if (s)  
		handle_error_en(s, "pthread_attr_getdetachstate");  
	printf("Detach state        = %s\n",
		(i == PTHREAD_CREATE_DETACHED) ? "PTHREAD_CREATE_DETACHED" :  
		(i == PTHREAD_CREATE_JOINABLE) ? "PTHREAD_CREATE_JOINABLE" :  
		"???");     

	 pthread_attr_destroy(&gattr);  //销毁线程属性union
}  
      
int main(int argc, char *argv[])  
{  
	pthread_t thr;  //pid
	int s;  
 
	s = pthread_create(&thr, NULL, &thread_start, NULL);  //创建线程
	if (s)  
	{
		handle_error_en(s, "pthread_create"); 
		return 0;
	}
	
	pthread_exit(NULL); //主线程退出，进程不立刻结束
    return 0;
} 