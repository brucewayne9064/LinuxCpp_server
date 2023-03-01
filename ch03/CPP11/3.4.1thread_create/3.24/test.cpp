#include <iostream>  
#include <thread>  
using namespace std;  
   
struct MYSTRUCT //定义结构体的类型
{
	int n;
	const char *str;
};

void thfunc(void *arg)  //线程函数
{  
	MYSTRUCT *p = (MYSTRUCT*)arg;  //把传进来的结构体的指针赋给p
	cout << "in thfunc:n=" << p->n<<",str="<< p->str <<endl; //打印结构体的内容
}  
      
int main(int argc, char *argv[])  
{  
	MYSTRUCT mystruct; //定义结构体
	//初始化结构体
	mystruct.n = 110;
	mystruct.str = "hello world";

	thread t(thfunc, &mystruct);  //定义线程对象t，并把线程函数指针和线程函数参数传入
	t.join();  //等待线程对象t结束
      
	return 0;  
}  
