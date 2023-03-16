#include <cstdio>
#include<sys/socket.h>  
#include <arpa/inet.h>
#include<string.h>
#include <errno.h>
 
int main()
{
	int sfp;
	struct sockaddr_in s_add;
	unsigned short portnum = 10051;  //端口号
	struct sockaddr_in serv = { 0 };  //本地套接字地址的结构体
	char on = 1;
	int serv_len = sizeof(serv); 
	int err;
	sfp = socket(AF_INET, SOCK_STREAM, 0);  //创建套接字，ipv4，流套接字类型，tcp
	if (-1 == sfp)
	{
		printf("socket fail ! \r\n");
		return -1;
	}
	printf("socket ok !\n");
    getsockname(sfp, (struct sockaddr *)&serv, (socklen_t*)&serv_len); //获取本地套接字地址
	printf("ip=%s,port=%d\n", inet_ntoa(serv.sin_addr), ntohs(serv.sin_port)); //打印没绑定前的地址
	setsockopt(sfp, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));//将sfp套接字的SO_REUSEADDR选项设为1，即开启，运行地址重用，避免在关闭套接字后再次绑定同一地址出错，防止端口占用
	memset(&s_add, 0, sizeof(struct sockaddr_in));  //和s_add = {0};效果一样
	s_add.sin_family = AF_INET;  //ipv4
	s_add.sin_addr.s_addr = inet_addr("192.168.0.118"); //这个ip地址必须是本机上有的,转换为网络字节序
	s_add.sin_port = htons(portnum);//端口号转换为网络字节序

	if (-1 == bind(sfp, (struct sockaddr *)(&s_add), sizeof(struct sockaddr)))  //s_add绑定sfp
	{
		printf("bind fail:%d!\r\n", errno);
		return -1;
	}
	printf("bind ok !\n");
	getsockname(sfp, (struct sockaddr *)&serv, (socklen_t*)&serv_len); //获取本地套接字地址
    //打印套接字地址里的ip和端口值
	printf("ip=%s,port=%d\n", inet_ntoa(serv.sin_addr), ntohs(serv.sin_port));
    return 0;
}