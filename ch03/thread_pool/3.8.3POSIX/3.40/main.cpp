#include"thread_pool.h"
#include<iostream>
#include<unistd.h>
#include<string>

class CMytask : public CTask {
public:
	CMytask() = default;

	~CMytask() {};

	int Run() {
		std::cout << this->m_ptrData << std::endl;
		int x = rand() % 4 + 1;
		sleep(x);  //随机休眠1~4s
		return 0;
	}
};

int main(int argc, char *argv[])
{
	CMytask taskObj;
	string s = "hello!";
	taskObj.setData(s);
	CThreadPool threadpool(5);  //创建一个五个线程的线程池
	for(int i = 0; i < 10; i++)
	{
		threadpool.AddTask(&taskObj);  //往任务链上添加十个task
	}
	while(true)
	{
		std::cout << "还有" << threadpool.getTaskSize() << "个任务需要被处理" << std::endl;
		if(threadpool.getTaskSize() == 0)
		{
			if(threadpool.StopAll() == -1)
			{
				std::cout << "线程池已经销毁，退出" << std::endl;
				exit(0);
			}
				
		}
		sleep(2);
		std::cout << "两秒钟后————" << std::endl;
	}


	return 0;
}

