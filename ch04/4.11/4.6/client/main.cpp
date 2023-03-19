#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <unistd.h>
#include <cstring>
#define BUF_LEN 1000



int main(int argc, char * argv[])
{
    int sockClient = socket(AF_INET, SOCK_STREAM, 0);


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


    char recvBuf[BUF_LEN];
    int iRes;

    do{
        iRes = recv(sockClient, recvBuf, BUF_LEN, 0);
        if (iRes > 0)
        {
            std::cout << "接收到了服务器的 " << iRes << " bytes" << std::endl;
            for(int i  = 0; i < iRes ; i++)
            {
                printf("%c",recvBuf[i]);
            }
            printf("\n");
            
            memset(recvBuf, 0 , sizeof(recvBuf));
        }
        else if (iRes == 0)
        {
            std::cout << "服务器关闭了连接" << std::endl;
        }
        else{
            std::cout << "接收失败，错误代码为：" << errno << std::endl;
            close(sockClient);
            return 1;
        }
    } while(iRes > 0);

    char sendBuf[BUF_LEN];
    for (int i = 0; i < 10; i++)
    {
        sprintf(sendBuf, "NO.%d我是客户端\n", i+1);
        send(sockClient, sendBuf, strlen(sendBuf) + 1, 0);
        memset(sendBuf, 0 , sizeof(sendBuf));
    }


    puts("向服务器发送数据已经完成！");
 
    close(sockClient);
    getchar();

    return 0;
}