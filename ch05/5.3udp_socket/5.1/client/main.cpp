#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

int main (int argc, char *argv[])
{
    char on = 1;

    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = inet_addr("121.48.161.76");
    saddr.sin_port = htons(9999);


    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd < 0)
    {
        perror("failed socket");
        return -1;
    }

    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));  //允许端口复用

    puts("请输入数据：");
    char wbuf[50];
    scanf("%s", wbuf);
    int ret = sendto(sockfd, wbuf, sizeof(wbuf), 0, (sockaddr *)&saddr, sizeof(struct sockaddr));
    if(ret < 0)
    {
        perror("sendto failed");
    }

    sockaddr_in local = {0};
    int len = sizeof(local);
    getsockname(sockfd, (sockaddr *)&local, (socklen_t*)&len);
    printf("local ip: %s\n", inet_ntoa(local.sin_addr));

    close(sockfd);

    getchar();
    return 0;
}