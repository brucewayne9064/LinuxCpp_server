#include <iostream>  
#include <pthread.h>  
#include <signal.h>  
#include <unistd.h> //sleep
using namespace std;

void *thfunc(void *arg)
{
    int *time = (int *)arg;
	int tm = *time;
	while (tm >= 0) 
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

    int time = *argv[1] - '0';
	
	res = pthread_create(&pid, NULL, thfunc, &time);  //创建子线程
	
	sleep(5);  //子线程执行时间
	
	int kill_rc = pthread_kill(pid, 0);  //向子线程发送0信号，用于检测线程是否结束
    if (kill_rc == ESRCH)
        std::cout << "线程不存在" << std::endl;
    else if (kill_rc == EINVAL)
        std::cout << "信号不合法" << std::endl;
    else
        std::cout << "线程存活，kill_rc的值为：" << kill_rc << std::endl;

	pthread_join(pid, NULL);   //等待子线程结束
	cout << "sub thread has completed,main thread will exit\n";
	 
	return 0;
}