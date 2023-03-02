
#include <iostream>       // std::cout
#include <thread>         // std::thread, std::this_thread::yield
#include <atomic>         // std::atomic
using namespace std;

atomic<bool> ready(false);  //设置一个原子类的布尔值 ，全局

void thfunc(int id) 
{
	while (!ready) //一直等待，直到main线程中重置全局变量ready            
		 this_thread::yield();  //让出自己的cpu时间片
 
	for (volatile int i = 0; i < 1000000; ++i)   //累加到百万
	{}
	 cout << id<<",";//输出的是排名，先完成先打印
}

int main()
{
	 thread threads[10];  //定义十个线程对象
	 cout << "race of 10 threads that count to 1 million:\n";
	for (int i = 0; i < 10; ++i) 
		threads[i] = thread(thfunc, i);
	ready = true;                 
	for (auto& th : threads) th.join();  //等待十个线程结束
	 cout << '\n';

	return 0;
}