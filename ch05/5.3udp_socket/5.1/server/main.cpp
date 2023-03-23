#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>


int main(int argc, char *argv[])
{
    int sockfd, ret;
    char rbuf[50];
    char on = 1;
    struct sockaddr_in saddr;  //服务器地址
    struct sockaddr_in raddr;  //客户端地址


    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = inet_addr("121.48.161.76");
    saddr.sin_port = htons(9999);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd < 0) {
        perror("socket failed");
        return -1;
    }

    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));  //地址复用

    ret = bind(sockfd, (struct sockaddr*)&saddr, sizeof(struct sockaddr));
    if(ret < 0) {
        perror("sbind failed");
        return -1;
    }    

    puts("waiting data");
    int val = sizeof(struct sockaddr);
    ret = recvfrom(sockfd, rbuf, sizeof(rbuf), 0, (struct sockaddr*)&raddr, (socklen_t*)&val);
    if(ret < 0) {
        perror("recvfrom failed");
    }        
    printf("recv data : %s from ip: %s\n", rbuf, inet_ntoa(raddr.sin_addr));

    close(sockfd);
    getchar();


    return 0;
}