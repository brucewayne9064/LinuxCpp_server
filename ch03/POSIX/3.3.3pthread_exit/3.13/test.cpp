#include <iostream>  
#include <pthread.h>  
#include <signal.h>  
#include <unistd.h> //sleep
using namespace std;

static void on_signal_term(int sig)  //信号处理函数
{
	cout << "sub thread will exit" << endl;
	pthread_exit(NULL); 
}
void *thfunc(void *arg)
{
	  // signal(SIGQUIT, on_signal_term);  //注册信号处理函数
 
	int tm = 50;
	while (tm >= 0) //运行50秒，第5秒收到SIGQUIT退出
	{
		cout << "thrfunc--left:"<<tm<<" s--" <<endl;
		sleep(1);
		tm--;
	}
	
	return (void *)0;   
}

int main(int argc, char *argv[])
{
	pthread_t     pid;  
	int res;
	
	res = pthread_create(&pid, NULL, thfunc, NULL);  //创建子线程
	
	sleep(5);  //子线程执行时间
	
	pthread_kill(pid, SIGQUIT);  //向子线程发送SIGQUIT信号
	pthread_join(pid, NULL);   //等待子线程结束
	cout << "sub thread has completed,main thread will exit\n";
	 
	return 0;
}