#include <iostream>       // std::cout
#include <thread>         // std::this_thread::sleep_until
#include <chrono>         // std::chrono::system_clock
#include <ctime>          // std::time_t, std::tm, std::localtime, std::mktime
#include <time.h>
#include <stddef.h>
using namespace std;

void getNowTime()
{
	timespec time;  //timespec结构体按照​秒(​tv_sec)和纳秒(​tv_nsec)​来定义时间
	tm nowTime;
	clock_gettime(CLOCK_REALTIME, &time);  //参数为（clockid_t clk_id,struct timespec *tp）获取相对于1970到现在的秒数
	                                       //clk_id : 检索和设置的clk_id指定的时钟时间,CLOCK_REALTIME系统实时时间,从UTC1970-1-1 0:0:0开始计时
	localtime_r(&time.tv_sec, &nowTime);  //将simple calender time 转换为broken-down time,秒数转为日历
	char current[1024];
	printf(
		"%04d-%02d-%02d %02d:%02d:%02d\n",
		nowTime.tm_year + 1900,
		nowTime.tm_mon+1,
		nowTime.tm_mday, 
		nowTime.tm_hour,
		nowTime.tm_min,
		nowTime.tm_sec);
}
 
int main() 
{
	using std::chrono::system_clock;
	std::time_t tt = system_clock::to_time_t(system_clock::now());

	tm * ptm = localtime(&tt);
	getNowTime();  //打印现在的时间
    cout << "等到下一分钟的时候再继续\n";
	++ptm->tm_min; //加一分钟
	ptm->tm_sec = 0;  //秒数清零
	this_thread::sleep_until(system_clock::from_time_t(mktime(ptm)));  //暂停执行直到指定的时间点
	getNowTime();  
  
	return 0;
}