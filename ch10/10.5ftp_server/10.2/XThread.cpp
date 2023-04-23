#include "XThread.h"
#include "testUtil.h"
#include <thread>
#include <iostream>
#include "XTask.h"
#include <unistd.h>
#include <event2/event.h>



/*
 *  线程监听主线程对自己的通知（回调函数）
 当管道的读端有数据可读时，event_base会调用Notify_cb回调函数，并将this作为参数传递给它。
 */
static void Notify_cb(evutil_socket_t fd, short which, void *arg) {
	XThread *t = (XThread*)arg;
	t->Notify(fd, which);
}


void XThread::Notify(evutil_socket_t fd, short which) {
	testout(id << " thread At Notify()");
	char buf[2] = { 0 };
 
	int re = read(fd, buf, 1);
	if (re < 0)
		return;
	cout << id << " thread recv" << buf << endl;
}





//启动线程
void XThread::Start(){
    testout(this->id << " thread At Start()");  //打印出当前线程的id和一条信息
	this->Setup();
    std::thread th(&XThread::Main, this);  //创建一个thread对象，传入XThread类的Main方法和this指针作为参数
    th.detach(); //将本线程从线程调用中分离，独立运行
}

//线程启动要做的任务
void XThread::Main() {
	std::cout << this->id << " thread::Main() begin" << std::endl;
	event_base_dispatch(base);
	event_base_free(base);
	std::cout << this->id << " thread::Main() end" << std::endl;
}

/*
 *  安装线程
 */
bool XThread::Setup() {
	testout(id << " thread At Setup");

	int fds[2]; //fds[0]是读端，fds[1]是写端

	if (pipe(fds)) {
		cerr << "pipe failed" << endl;
		return false;
	}

	// 用于主线程通知子线程处理任务，windows用配对socket，linux用管道
	notify_send_fd = fds[1];

	// 创建lievent上下文
	event_config *ev_conf = event_config_new();
	event_config_set_flag(ev_conf, EVENT_BASE_FLAG_NOLOCK);  //给base添加属性：不为event base分配锁（因为只在单个线程中使用，所以不需要互斥访问）
	this->base = event_base_new_with_config(ev_conf);
	event_config_free(ev_conf);
	if (!base) {
		cout << "event_base_new_with_config error!" << endl;
		return false;
	}

	// 添加管道监听事件，用于线程池激活自己执行任务, 将this传入（不用再调用event_base_set函数）
	event *ev = event_new(base, fds[0], EV_READ | EV_PERSIST, Notify_cb, this);  //EV_READ | EV_PERSIST表示监听读事件并且持久化
	event_add(ev, 0);

	return true;
}


//线程激活（有任务）
void XThread::Activate() {
	testout(this->id << " thread At Activate()");

	int re = write(notify_send_fd, "c", 1);

	if (re <= 0) {
		cerr << "XThread::Activate() fail" << endl;
	}

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
	t->Init();  //任务初始化
}


//向任务列表添加任务
void XThread::AddTack(XTask *t) {
	if (!t) return;

	t->base = this->base;  //taskbase和线程base一样
	tasks_mutex.lock();
	tasks.push_back(t);   //
	tasks_mutex.unlock();
}

XThread::XThread() {
}

XThread::~XThread() { 

}
