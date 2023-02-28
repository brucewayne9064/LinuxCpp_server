#include <stdio.h>
#include <stdlib.h>

#include <chrono>    // std::chrono::seconds
#include <iostream>  // std::cout
#include <thread>    // std::thread, std::this_thread::sleep_for

void thfunc(int n)  //线程函数
{
	std::cout << "thfunc:" << n  <<  std::endl;
}
 
int main(int argc, const char *argv[])
{
	std::thread threads[5];   //批量实例化五个thread对象，无参构造？
	std::cout << "create 5 threads..." << std::endl;

	auto i = 1;
	for (auto& t : threads)
	{
		t = std::thread(thfunc, i); //右值引用，这里std::thread(thread_task, i + 1)为右值
		i = i + 1;
	}
	for (auto& t : threads)   //等待每个线程结束 (可连接线程)
		t.join();
 
	std::cout << "All threads joined." << std::endl; 

	return EXIT_SUCCESS;  
}   