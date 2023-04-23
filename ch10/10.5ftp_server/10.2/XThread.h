#pragma once
#include <event2/util.h>
#include <list>
#include <mutex>
class XTask;
struct event_base;


class XThread
{
public:
	void Start();// 启动线程
	void Main();// 线程入口函数
	bool Setup();// 安装线程，初始化evevnt_base和管道监听事件用于激活
	void Notify(evutil_socket_t, short);// 收到主线程发出的激活消息（线程池任务分发）
	void Activate();// 线程激活
	void AddTack(XTask *);// 添加任务, 一个线程可以同时处理多个任务，共用一个event_base
	XThread();  //构造函数
	~XThread();  //析构函数
	int id = 0;// 线程编号

private:
	int notify_send_fd = 0;
	event_base *base = 0;
	std::list<XTask*> tasks;  //任务列表，每个线程都有一个任务列表
	std::mutex tasks_mutex;  //访问任务列表的互斥信号
};

/* 线程和任务都要创建一个base指针：event_base *base，是因为这个指针是libevent库中的一个重要的结构体，
它用来管理事件和回调函数。每个线程都需要有自己的event_base结构体，以便在不同的线程中处理不同的事件。
而任务（task）是一种特殊的事件，它可以在任何线程中执行，但是它需要知道自己属于哪个线程，
所以要让task的base等于线程的base: t->base = this->base。这样就可以保证任务在正确的线程中运行，
并且可以访问该线程的其他资源。 */