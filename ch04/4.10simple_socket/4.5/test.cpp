#include <cstdio>
#include <assert.h>
#include <sys/time.h>
#include <sys/types.h>          
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define BUFFER_SIZE 512
// 返回自系统开机以来的毫秒数（tick）
unsigned long GetTickCount()
{
	struct timeval tv;
	if (gettimeofday(&tv, NULL) != 0)
		return 0;

	return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}
int main(int argc, char * argv [])
{
	char ip[] = "192.168.239.88"; //该IP是和本机通一网段的地址 ，但并不存在  
	int err,port = 13334;
	struct sockaddr_in server_address;
	memset(&server_address, 0, sizeof(server_address));  //清空server_address
	server_address.sin_family = AF_INET;  //ipv4
	server_address.sin_addr.s_addr = inet_addr(ip);  //ip
	server_address.sin_port = htons(port);  //端口13334

	int sock = socket(PF_INET, SOCK_STREAM, 0);  //产生套接字
	assert(sock >= 0);
	long t1 = GetTickCount();//得到现在的时间
	int ret = connect(sock, (struct sockaddr*)&server_address, sizeof(server_address));//连接服务器
	if (ret == -1)
	{
		long t2 = GetTickCount();  //得到现在的时间
		printf("connect failed: %d\n", ret);
		printf("time used:%dms\n", t2 - t1);
		if (errno == EINPROGRESS)  //非阻塞下的返回值，但是现在是默认的阻塞模式所以这个不会用到
		{
			printf("unblock mode ret code...\n");
		}
	}
	else printf("ret code is: %d\n", ret);
	close(sock);
	getchar();
	return 0;
}
