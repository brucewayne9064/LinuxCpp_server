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
#include <sys/ioctl.h>
// 返回自系统开机以来的毫秒数（tick）
unsigned long GetTickCount()
{
	struct timeval tv;
	if (gettimeofday(&tv, NULL) != 0) return 0;
	return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}
int main()
{
	int argp;
	int res;
	char ip[] = "192.168.239.88"; //该ip是和本机通一网段的地址 ，但并不存在  
	int port = 13334;
	struct sockaddr_in server_address;
	memset(&server_address, 0, sizeof(server_address));
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = inet_addr(ip);
	server_address.sin_port = htons(port);

	int sock = socket(PF_INET, SOCK_STREAM, 0);
	assert(sock >= 0);
	long t1 = GetTickCount();  //获取当前时间
	int ret = connect(sock, (struct sockaddr*)&server_address, sizeof(server_address));  //connect一个不存在的地址
	printf("connect ret code is: %d\n", ret);
	if (ret == -1)
	{
		long t2 = GetTickCount();  //获取返回失败代码的时间
		printf("time used:%dms\n", t2 - t1);
		printf("connect failed...\n");
		if (errno == EINPROGRESS)  //如果返回EINPROGRESS，说明这是非阻塞模式
			printf("unblock mode ret code...\n");
	}
	else	printf("ret code is: %d\n", ret);
	argp = 1;
	res = ioctl(sock, FIONBIO, &argp);  //argp为1，说明设置为非阻塞模式
	if (-1 == res)
	{
		printf("Error at ioctlsocket(): %ld\n", errno);
		return -1;
	}
	puts("\nAfter setting non blocking mode:");
	memset(&server_address, 0, sizeof(server_address));
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = inet_addr(ip);
	server_address.sin_port = htons(port);

	t1 = GetTickCount();  //获取当前时间

	ret = connect(sock, (struct sockaddr*)&server_address, sizeof(server_address));
	printf("connect ret code is: %d\n", ret);
	if (ret == -1)
	{
		long t2 = GetTickCount();  //返回连接失败的时间
		printf("time used:%dms\n", t2 - t1);
		if (errno == EINPROGRESS)
		{
			printf("unblock mode errno:%d\n", errno);
			//后续可以用select函数来判断连接是否成功。
		}
	}
	else 	printf("ret code is: %d\n", ret);
	close(sock);
	getchar();
	return 0;
}