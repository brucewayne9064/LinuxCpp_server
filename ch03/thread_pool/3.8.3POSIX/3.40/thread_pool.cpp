#include "thread_pool.h"
#include <cstdio>

//执行任务类的设置任务数据函数
void CTask::setData(void* data) {
	this->m_ptrData = data;
}

//静态成员初始化
vector<CTask*> CThreadPool::m_vecTaskList;
bool CThreadPool::shutdown = false;
pthread_mutex_t CThreadPool::m_pthreadMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t CThreadPool::m_pthreadCond = PTHREAD_COND_INITIALIZER;

//线程管理类构造函数，输入要创建的线程数量，用create函数创造
CThreadPool::CThreadPool(int threadNum) {
	this->m_iThreadNum = threadNum;
	printf("I will create %d threads.\n", threadNum);
	Create();
}


//创建线程，创建m_iThreadNum个
int CThreadPool::Create() { 
	pthread_id = new pthread_t[m_iThreadNum];
	for (int i = 0; i < m_iThreadNum; i++)
		pthread_create(&pthread_id[i], NULL, ThreadFunc, NULL);

	return 0;
}

//线程回调函数
/* 这个函数是线程池中的线程执行的函数，它的作用是从任务队列中取出
一个任务并执行，然后等待下一个任务。 */
void* CThreadPool::ThreadFunc(void* threadData) {
	pthread_t tid = pthread_self();  //本线程的id
	while (1)  //不断检查任务队列
	{
		pthread_mutex_lock(&m_pthreadMutex);  //互斥访问
		//如果队列为空，且没有收到关闭信号，等待新任务进入任务队列
		while (m_vecTaskList.size() == 0 && !shutdown)
			pthread_cond_wait(&m_pthreadCond, &m_pthreadMutex);
        
		//关闭线程
		if (shutdown)
		{
			pthread_mutex_unlock(&m_pthreadMutex);
			printf("[tid: %lu]\texit\n", tid);
			pthread_exit(NULL);
		}
        
		printf("[tid: %lu]\trun: ", tid);  //打印本线程的id和运行状态
		vector<CTask*>::iterator iter = m_vecTaskList.begin();
		//取出一个任务并处理之
		CTask* task;
		if (iter != m_vecTaskList.end())
		{
			task = *iter;  //取出第一个任务，并从队列中删掉
			m_vecTaskList.erase(iter);
		}
        
		pthread_mutex_unlock(&m_pthreadMutex);  //结束对队列的互斥访问
        
		task->Run();    //执行任务
		printf("[tid: %lu]\tidle\n", tid);//打印本线程的id和空闲状态
        
	}
    
	return (void*)0;
}

//往任务队列里添加任务并发出线程同步信号
int CThreadPool::AddTask(CTask *task) { 
	pthread_mutex_lock(&m_pthreadMutex);    
	m_vecTaskList.push_back(task);  
	pthread_mutex_unlock(&m_pthreadMutex);  
	pthread_cond_signal(&m_pthreadCond);    
    
	return 0;
}



//停止所有线程
int CThreadPool::StopAll() {    
    //避免重复调用
	if (shutdown)
		return -1;
	printf("Now I will end all threads!\n\n");
    
	//唤醒所有等待进程，线程池也要销毁了
	shutdown = true;
	pthread_cond_broadcast(&m_pthreadCond);
    
	//清除僵尸线程
	for (int i = 0; i < m_iThreadNum; i++)
		pthread_join(pthread_id[i], NULL);
    
	delete[] pthread_id;
	pthread_id = NULL;
    
	//销毁互斥量和条件变量
	pthread_mutex_destroy(&m_pthreadMutex);
	pthread_cond_destroy(&m_pthreadCond);
    
	return 0;
}

//获取当前队列中的任务数
int CThreadPool::getTaskSize() {    
	return m_vecTaskList.size();
}