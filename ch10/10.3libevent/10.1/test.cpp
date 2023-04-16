#include <sys/types.h>
#include <event2/event-config.h>
#include <stdio.h>
#include <event.h>
//事件源是系统的时钟，它会在每个超时时间到达时触发一个定时器事件
struct event ev;  //事件
struct timeval tv;
 
void time_cb(int fd, short event, void *argc) //具体事件处理器
{
	printf("timer wakeup!\n");
	event_add(&ev, &tv);  //重新把定时器事件添加到事件循环中，让它在下一个10秒后触发。这样就实现了一个周期性的定时器
}           
            
int main()  
{           
	struct event_base *base = event_init(); //初始化  初始分发器
	tv.tv_sec = 10;
	tv.tv_usec = 0; 
	evtimer_set(&ev, time_cb, NULL);  //初始化   事件处理器
	event_base_set(base, &ev);  //把event注册到base
	event_add(&ev, &tv);  //设置超时器，10s后系统就会触发该事件, 把event注册到同步事件分离器
	event_base_dispatch(base);  //启动事件循环，监听注册到base中且注册到多路复用中的事件，那些只注册到base而没有注册到多路复用的事件不监听
}  
 

 /* 
 工作流程大致如下：

你创建一个事件基础结构，用来管理所有的事件。
你创建一个定时器事件，用来在每个超时时间到达时执行一个回调函数。
你把定时器事件添加到事件基础结构中，让它参与事件循环。
你启动事件循环，让程序进入阻塞等待状态。
当系统的时钟触发一个定时器事件时，同步事件分离器会返回这个事件，初始分发器会把它分配给time_cb函数，time_cb函数会打印一条信息，并且重新把定时器事件添加到事件基础结构中。
这个过程会不断重复，直到你停止事件循环或者程序退出。
已收到消息. 工作流程大致如下： 1. 你创建一个事件基础结构，用来管理所有的事件。 2. 你创建一个定时器事件，用来在每个超时时间到达时执行一个回调函数。 3. 你把定时器事件添加到事件基础结构中，让它参与事件循环。 4. 你启动事件循环，让程序进入阻塞等待状态。 5. 当系统的时钟触发一个定时器事件时，同步事件分离器会返回这个事件，初始分发器会把它分配给time_cb函数，time_cb函数会打印一条信息，并且重新把定时器事件添加到事件基础结构中。 6. 这个过程会不断重复，直到你停止事件循环或者程序退出。

  */