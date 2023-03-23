#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>

int main (int argc, char *argv[])
{
    char on = 1;

    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = inet_addr("192.168.239.128");
    saddr.sin_port = htons(8888);


    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd < 0)
    {
        perror("failed socket");
        return -1;
    }

    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));  //允许端口复用


    char wbuf[50];
    while(true)
    {
        puts("请输入数据：");


        fgets(wbuf, sizeof(wbuf), stdin);
        int ret = sendto(sockfd, wbuf, sizeof(wbuf), 0, (sockaddr *)&saddr, sizeof(struct sockaddr));
        if(ret < 0)
        {
            perror("sendto failed");
        }
        memset(wbuf, 0 , sizeof(wbuf));
    }

    close(sockfd);

    getchar();
    return 0;
}