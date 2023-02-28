#include <iostream>  
#include <thread>  
      
using namespace std;  
      
void th_function()  
{  
	std::cout << "i am c++11 thread func" << std::endl;  
}  
      
int main(int argc, char *argv[])  
{  
	std::thread t(th_function);  //定义线程对象，传入线程函数指针
	t.join();  
      
	return 0;  
}   