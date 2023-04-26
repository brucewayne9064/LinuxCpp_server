#include "XThread.h"
#include "testUtil.h"
#include <thread>
#include <iostream>
#include "XTask.h"


//启动线程
void XThread::Start(){
    testout(this->id << " thread At Start()");  //打印出当前线程的id和一条信息
    std::thread th(&XThread::Main, this);  //创建一个thread对象，传入XThread类的Main方法和this指针作为参数
    th.detach(); //将本线程从线程调用中分离，独立运行
}

//线程启动要做的任务
void XThread::Main() {
	std::cout << this->id << " thread::Main() begin" << std::endl;

	std::cout << this->id << " thread::Main() end" << std::endl;
}


//线程激活（有任务）
void XThread::Activate(int arg) {
	testout(this->id << " thread At Activate()");

    //从列表中获取任务并初始化
	XTask *t = NULL;  //创建一个XTask类型的指针，用于接下来指向任务
	this->tasks_mutex.lock();//互斥访问列表
	if (this->tasks.empty()) {
		this->tasks_mutex.unlock();
		return;
	}
	t = this->tasks.front();
	this->tasks.pop_front();
	this->tasks_mutex.unlock(); //取出一个任务给t并解锁
	t->task_Init(arg);  //任务初始化
}


//向任务列表添加任务
void XThread::AddTack(XTask *t) {
	if (!t) return;

	t->base = this->base;  //什么意思
	tasks_mutex.lock();
	tasks.push_back(t);
	tasks_mutex.unlock();
}

XThread::XThread() {
}

XThread::~XThread() { 

}
