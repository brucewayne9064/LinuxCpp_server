#pragma once
class XTask
{
public:
	// 一客户端一个base
	struct event_base *base = 0;

	// 连接的sock
	int sock = 0;

	// 线程池id
	int thread_id = 0;

	// 初始化任务
	virtual bool Init() = 0; //虚函数是为了让继承的子类对其进行重写，从而实现多态
};