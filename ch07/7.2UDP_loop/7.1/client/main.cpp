#include <stdio.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8888
char wbuf[50], rbuf[100];
int main(int argc, char* argv[])
{
    int on = 1;

	struct sockaddr_in saddr, raddr;
	memset(&saddr, 0, sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(PORT);
	saddr.sin_addr.s_addr = inet_addr("192.168.239.128");


	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd < 0)
    {
        perror("failed socket");
        return -1;
    }
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));  //允许端口复用

	int fromlen = sizeof(struct sockaddr);
	puts("请输入数据：");
    scanf("%s", wbuf);
	int ret = sendto(sockfd, wbuf, sizeof(wbuf), 0, (struct sockaddr*)&saddr, sizeof(struct sockaddr));
	if (ret < 0) perror("sendto failed");
	int len = recvfrom(sockfd, rbuf, sizeof(rbuf), 0, (struct sockaddr*)&raddr, (socklen_t *)&fromlen);
	if(len < 0) perror("recvfrom failed");
	printf("server reply:%s\n", rbuf);
 
	close(sockfd);
	getchar();
	return 0;
}
