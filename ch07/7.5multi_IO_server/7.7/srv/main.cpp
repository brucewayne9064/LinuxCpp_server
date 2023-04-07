#include <ctype.h>
#include <cstdio>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <errno.h>
#include <unistd.h> //for close
 
#define MAXLINE 80
#define SERV_PORT 8888
#define OPEN_MAX 1024

int main(int argc, char *argv[])
{
	int i, j, maxi, listenfd, connfd, sockfd;
	int nready, efd, res;
	ssize_t n;
	char buf[MAXLINE], str[INET_ADDRSTRLEN];
	socklen_t clilen;
	int client[OPEN_MAX];
	struct sockaddr_in cliaddr, servaddr;
	struct epoll_event tep, ep[OPEN_MAX];//tep存放监听套接字，ep存放epoll_wait返回的就绪的套接字

	//网络socket初始化
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	memset(&servaddr,0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);  //8888
	if(bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr))==-1)
		perror("bind error");
	if(listen(listenfd, 20) == -1)
		perror("listen error");
	puts("listen ok");


	for (i = 0; i < OPEN_MAX; i++)  //client数组全部初始化为-1
		client[i] = -1;


	maxi = -1;//后面数据初始化赋值时 数据初始化为-1
	efd = epoll_create(OPEN_MAX); //创建epoll描述符
	if (efd == -1)
		perror("epoll_create error");

	//添加监听套接字
	tep.events = EPOLLIN;
	tep.data.fd = listenfd;
	res = epoll_ctl(efd, EPOLL_CTL_ADD, listenfd, &tep);  //通过该函数把listenfd和要让它被监控的事件加入epoll的监控
	if (res == -1) perror("epoll_ctl error");



	for (; ; )
	{
		nready = epoll_wait(efd, ep, OPEN_MAX, -1);//阻塞监听
		if (nready == -1)
            perror("epoll_wait error");

		//如果有事件发生 开始数据处理
		for (i = 0; i < nready; i++)
		{
			//是否是读事件
			if (!(ep[i].events & EPOLLIN))  //如果不是读，跳过
				continue;

			//若处理的事件和文件描述符相等 数据处理
			if (ep[i].data.fd == listenfd)  //如果是监听描述符
			{
				//接收客户端
				clilen = sizeof(cliaddr);
				connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);
				printf("received from %s at PORT %d\n",
					inet_ntop(AF_INET, &cliaddr.sin_addr, str, sizeof(str)), ntohs(cliaddr.sin_port));
				for (j = 0; j < OPEN_MAX; j++)
					if (client[j] < 0)
					{
						//将通信套接字存放到client
						client[j] = connfd;
						break;
					}

				//是否到达最大值 保护判断
				if (j == OPEN_MAX)
					perror("too many clients");

				//更新client下标
				if (j > maxi)
					maxi = j;

				//添加通信套接字到epoll上
				tep.events = EPOLLIN;
				tep.data.fd = connfd;
				res = epoll_ctl(efd, EPOLL_CTL_ADD, connfd, &tep);
				if (res == -1)
					perror("epoll_ctl error");
			}
			else  //如果不是监听描述符
			{
				sockfd = ep[i].data.fd;//将connfd赋值给socket
				n = read(sockfd, buf, MAXLINE);//读取数据

				//无数据则删除该结点
				if (n == 0)
				{
					//将Client中对应fd数据值恢复为-1
					for (j = 0; j <= maxi; j++)
					{
						if (client[j] == sockfd)
						{
							client[j] = -1;
							break;
						}
					}
					res = epoll_ctl(efd, EPOLL_CTL_DEL, sockfd, NULL);//删除epoll树结点
					if (res == -1)
						perror("epoll_ctl error");

                        
					close(sockfd);
					printf("client[%d] closed connection\n", j);
				}
				else //有数据则写回数据
				{
					printf("recive client's data:%s\n",buf);
					//这里可以根据实际情况扩展，模拟对数据进行处理
					for (j = 0; j < n; j++)
						buf[j] = toupper(buf[j]); //现在简单的转为大写
					write(sockfd, buf, n); //回送给客户端
				}
			}
		}
	}
	close(listenfd);
	close(efd);
	return 0;
} 