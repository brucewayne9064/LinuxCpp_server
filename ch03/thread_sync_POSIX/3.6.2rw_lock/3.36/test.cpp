#include<iostream>
#include<pthread.h>
#include<sys/time.h>
#include<string.h>
using namespace std;

int gcn = 0;
pthread_mutex_t mutex;
pthread_rwlock_t rwlock;

void* Thread_1(void *arg)  
{
    int j;
    volatile int a;
	for(j = 0; j < 10000000; j++)
	{
        pthread_mutex_lock(&mutex);
		a = gcn;
        pthread_mutex_unlock(&mutex);
	}
    pthread_exit((void *)0);
}

void* Thread_2(void *arg)  
{
    int j;
    volatile int b;
	for(j = 0; j < 10000000; j++)
	{
        pthread_mutex_lock(&mutex);
		b = gcn;
        pthread_mutex_unlock(&mutex);
	}
    pthread_exit((void *)0);
}

void* Thread_3(void *arg)  
{
    int j;
    volatile int a;
	for(j = 0; j < 10000000; j++)
	{
        pthread_rwlock_rdlock(&rwlock);
		a = gcn;
        pthread_rwlock_unlock(&rwlock);
	}
    pthread_exit((void *)0);
}

void* Thread_4(void *arg)  
{
    int j;
    volatile int b;
	for(j = 0; j < 10000000; j++)
	{
        pthread_rwlock_rdlock(&rwlock);
		b = gcn;
        pthread_rwlock_unlock(&rwlock);
	}
    pthread_exit((void *)0);
}


int mutexVer(void)
{
    int j, err;
    pthread_t th1, th2;

    timeval start;  //记录秒和微秒的struct
    timeval end;
    clock_t t1, t2;

    pthread_mutex_init(&mutex, NULL);  //初始化互斥锁

    gettimeofday(&start, NULL);  //返回当前距离1970年的秒数和微妙数，第二个参数是时区

    err = pthread_create(&th1, NULL, Thread_1, (void*)0);
    if(err != 0)
    {
        cout << "线程创建错误：" << strerror(err) << endl;
        exit(0);
    }

    err = pthread_create(&th2, NULL, Thread_2, (void*)0);
    if(err != 0)
    {
        cout << "线程创建错误：" << strerror(err) << endl;
        exit(0);
    }

    err = pthread_join(th1, NULL);
    if(err != 0)
    {
        cout << "等待线程结束错误：" << strerror(err) << endl;
        exit(1);
    }

    err = pthread_join(th2, NULL);
    if(err != 0)
    {
        cout << "等待线程结束错误：" << strerror(err) << endl;
        exit(1);
    }

    gettimeofday(&end, NULL);  //返回当前距离1970年的秒数和微妙数，第二个参数是时区

    pthread_mutex_destroy(&mutex);  //销毁互斥锁

    long long total_time = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);
    total_time = total_time / 1000;  //转换成毫秒
    cout << "mutex花费的总时间为：" << total_time << "ms" << endl;
    return 0;
}

int rdlockVer(void)
{
    int j, err;
    pthread_t th1, th2;

    timeval start;  //记录秒和微秒的struct
    timeval end;
    clock_t t1, t2;

    pthread_rwlock_init(&rwlock, NULL);  //初始化读写锁

    gettimeofday(&start, NULL);  //返回当前距离1970年的秒数和微妙数，第二个参数是时区

    err = pthread_create(&th1, NULL, Thread_3, (void*)0);
    if(err != 0)
    {
        cout << "线程创建错误：" << strerror(err) << endl;
        exit(0);
    }

    err = pthread_create(&th2, NULL, Thread_4, (void*)0);
    if(err != 0)
    {
        cout << "线程创建错误：" << strerror(err) << endl;
        exit(0);
    }

    err = pthread_join(th1, NULL);
    if(err != 0)
    {
        cout << "等待线程结束错误：" << strerror(err) << endl;
        exit(1);
    }

    err = pthread_join(th2, NULL);
    if(err != 0)
    {
        cout << "等待线程结束错误：" << strerror(err) << endl;
        exit(1);
    }

    gettimeofday(&end, NULL);  //返回当前距离1970年的秒数和微妙数，第二个参数是时区

    pthread_rwlock_destroy(&rwlock);  //销毁读写锁

    long long total_time = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);
    total_time = total_time / 1000;  //转换成毫秒
    cout << "rwlock花费的总时间为：" << total_time << "ms" << endl;
    return 0;
}

int main(int argc, char *argv [])
{
    mutexVer();
    rdlockVer();
    return 0;
}