#include <iostream>       // std::cout
#include <thread>         // std::thread
#include <mutex>          // std::mutex

volatile int counter(0); // non-atomic counter
std::mutex mtx;           // locks access to counter

void thrfunc() 
{
	for (int i = 0; i < 10000; ++i) //计数到一万
	{
        if(mtx.try_lock())
        {
		    ++counter;
		    mtx.unlock();
        }
        else std::cout << "尝试上锁失败" << std::endl;
		 
	}
}

int main(int argc, const char* argv[]) 
{
	std::thread threads[10];  //创建十个线程对象
	
	for (int i = 0; i < 10; ++i)
		threads[i] = std::thread(thrfunc);

	for (auto& th : threads) th.join();
	std::cout <<"count to "<< counter << " successfully \n";

	return 0;
}