#pragma once
class XTask
{
public:
	// 一客户端一个base
	struct event_base *base = 0;
	int thread_id = 0; // 线程池id
	// 初始化任务
	virtual bool task_Init(int arg) = 0;//具体如何初始化，就要根据具体的任务而重载
};