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
	std::thread threads[5];   //实例化五个thread对象，此时不执行
	std::cout << "create 5 threads..." << std::endl;
	for (int i = 0; i < 5; i++) 
		threads[i] = std::thread(thfunc, i + 1);  //执行线程函数thfunc
	 
	for (auto& t : threads)   //等待每个线程结束 
		t.join();
 
	std::cout << "All threads joined." << std::endl; 

	return EXIT_SUCCESS;  
}   