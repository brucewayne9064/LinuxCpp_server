#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    pid_t pid;
    pid = fork();
    if( pid == -1){
        perror("can not fork");
        return -1;
    }

    else if (pid == 0)
    {
        printf("this is child process\n");
        printf("pid is %d, my PID is %d\n", pid , getpid());
    }
    else 
    {
        printf("this is parent process\n");
        printf("child pid is %d, my PID is %d\n", pid , getpid());
    }
    return 0;
}