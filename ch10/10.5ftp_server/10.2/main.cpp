#include <iostream>
#include <signal.h>
#include <stdlib.h>
using std::cout;
using std::cin;
using std::endl;
using std::cerr;
using std::flush;
using std::string;

#define errmsg(msg) do{cout<< msg << endl;exit(1);}while(0)  //错误处理函数

#include <string.h>
#include <string>
#include <fstream>

#include <event2/event.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/http.h>
#include <event2/keyvalq_struct.h>
#include <event2/buffer.h>
#include <event2/util.h>


#include "testUtil.h"
#include "XThreadPool.h"
#include "XThread.h"
#include "XTask.h"
#include "XFtpFactory.h"


#define SPORT 21
#define BUFS 1024

#define XThreadPoolGet XThreadPool::Get()

//接待连接的回调函数,如果有连接，就会通知初始分发器调用该函数
void listen_cb(struct evconnlistener *ev, evutil_socket_t s, struct sockaddr *addr, int socklen, void *arg) {
	testout("main thread At listen_cb");
	sockaddr_in *sin = (sockaddr_in*)addr;

	//创建任务
	XTask *task = XFtpFactory::Get()->CreateTask();

    //s为客户端套接字
	task->sock = s;

	//分配任务
	XThreadPoolGet->Dispatch(task);
}

int main(int argc, char* argv[]) {

 
	if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
        errmsg("signal(SIGPIPE, SIG_IGN) error!");
 


    //初始化线程池
	XThreadPoolGet->Init(10);

    //创建base，即初始事件分发器
	event_base *base = event_base_new();
	if (!base)
		errmsg("main thread event_base_new error");

    //建立ftp监听地址
	sockaddr_in sin;  //ftp服务器监听地址
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;  // ipv4
	sin.sin_port = htons(SPORT);  //监听端口是21号

    //创建监听事件，内部已经调用了event_base_set和event_add
	evconnlistener *ev = evconnlistener_new_bind(
		base,											//初始事件分发器
		listen_cb,										//接收到连接的回调函数
		base,											//回调函数获取的参数 arg，这里传递了base指针
		LEV_OPT_REUSEABLE|LEV_OPT_CLOSE_ON_FREE,		//地址重用，evconnlistener关闭同时关闭socket
		10,												//连接队列大小，对应listen函数
		(sockaddr*)&sin,								//绑定的地址和端口
		sizeof(sin));                                   //sockaddr_in结构体的大小

	if (base) {
		cout << "begin to listen..." << endl;
		event_base_dispatch(base);           //启动事件循环，监听注册到base中且注册到demultiplexer中的事件
	}
	if (ev)
		evconnlistener_free(ev);  //关闭监听套接字并释放内存
	if (base)
		event_base_free(base);  //释放事件循环的内存
	testout("server end");
	return 0;
}

