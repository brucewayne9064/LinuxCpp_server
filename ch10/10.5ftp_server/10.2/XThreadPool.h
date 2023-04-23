#pragma once
#include <vector>

class XThread;
class XTask;
class XThreadPool
{
public:
	// 单例模式，单例模式是一种设计模式，它保证一个类只有一个实例，并提供一个全局访问点
	static XThreadPool *Get() {
		static XThreadPool p;
		return &p;
	}
	// 初始化所有线程
	void Init(int threadCount);

	// 分发线程
	void Dispatch(XTask*);
private:
	int threadCount;  //线程池中的线程总数
	int lastThread = -1;
	std::vector<XThread *> threads;//所有线程的向量，一共十个
	XThreadPool() {};
};

/* 这段代码定义了一个名为 XThreadPool 的类，它有一个静态成员变量 p ，用来存储唯一的实例。它的构造函数和拷贝构造函数都设置为私有的，这样就防止了外部通过 new 或者复制来创建新的实例。

它还提供了一个静态方法 Get() ，用来返回唯一的实例。2 这个方法使用了静态局部变量 p ，它在第一次调用时被初始化，并且在程序结束时被销毁。这样就保证了线程安全和资源管理。

所以，这段代码的意思是，如果你想使用 XThreadPool 类的对象，你只能通过调用 XThreadPool::Get() 方法来获取，而不能自己创建新的对象。 */
