#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
int main(int argc, char * argv[])
{
    char *p1 = "this is a c test code, \
                this is a c test code, \
                this is a c test code, \
                this is a c test code, \
                this is a c test code, \
                this is a c test code, \
                this is a c test code, \
                this is a c test code, \
                this is a c test code, \
                this is a c test code, \
                this is a c test code, \
                this is a c test code, \
                this is a c test code, \
                this is a c test code, \
                this is a c test code, \
                this is a c test code";
    /* volatile是一个关键字，它告诉编译器不要对这个变量进行优化。
    这意味着每次访问这个变量时，都会从内存中读取它的值，而不是使用
    缓存的值。这在多线程编程中很有用，因为它可以确保多个线程对该变
    量的访问是同步的。 */
    volatile int len = 0;

    int fp = open("./test.txt", O_RDWR | O_CREAT);  //以读写模式打开，如果文件不存在则创建
    for(;;)
    {
        int n;
        if((n = write(fp, p1+ len, strlen(p1) - len)) == 0)
        {
            printf(" last n = %d \n", n);
            break;
        }
        else{
            printf(" n = %d \n", n);
        }
        len += n;
    }
    return 0;
}