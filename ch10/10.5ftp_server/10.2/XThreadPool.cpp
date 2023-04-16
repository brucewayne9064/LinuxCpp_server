#include "XThreadPool.h"
#include "testUtil.h"
#include "XThread.h"
#include <iostream>
#include <chrono>
#include <thread>



//分配任务到线程池
void XThreadPool::Dispatch(XTask *task,int arg) {
	testout("main thread At XThreadPoll::dispathch()");
	
	if (!task) return;
	int tid = (lastThread + 1) % threadCount;
	lastThread = tid;
	XThread *t = threads[tid];//得到最新线程的指针，从线程池里拿一个线程
	
	// 添加任务
	t->AddTack(task);
	// 激活线程
	t->Activate(arg);
}


//初始化线程池
void XThreadPool::Init(int threadCount) {
	testout("main thread At XThreadPoll::Init()");
	this->threadCount = threadCount;  //10个线程
	this->lastThread = -1;
	for (int i = 0; i < threadCount; i++) {
		std::cout << "Create thread" << i << std::endl;
		XThread *t = new XThread();
		t->id = i;
		t->Start();
		this->threads.push_back(t);
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}