#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <unistd.h>
#include <cstring>




int main(int argc, char * argv[])
{
    int sockClient = socket(AF_INET, SOCK_STREAM, 0);
    char msg[] = "hi, server";


    sockaddr_in addrSrv;
    addrSrv.sin_addr.s_addr = inet_addr("192.168.239.128");
    addrSrv.sin_family = AF_INET;
    addrSrv.sin_port = htons(8000);


    int err;
    err = connect(sockClient, (sockaddr *) &addrSrv, sizeof(sockaddr));
    if (err == -1)
    {
        std::cout << "连接服务器失败，请检查服务器是否开启" << std::endl;
        return 0;
    }


    char recvBuf[100];
    recv(sockClient, recvBuf, 100, 0);
    std::cout << "收到的服务器的信息： " << recvBuf << std::endl;
    send(sockClient, msg, strlen(msg) + 1, 0);
    close(sockClient);
    getchar();

    return 0;
}