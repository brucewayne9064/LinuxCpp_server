#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <string>
#include <cstring>
#include <cassert>
#define BUF_LEN 300


struct MyData{
    int nLen;
    char data[0];
};

int main(int argc , char *argv [])
{
    int serv_len, client_len;
    int iRes;
    char on = 1;  //char类型只占一个字节的空间，int占4个
    struct sockaddr_in addrSrv, serv, addrClient;  //socket地址


    int sockSrv = socket(AF_INET, SOCK_STREAM, 0);  //创建套接字，用于服务器监听
    assert(sockSrv >=0);

    setsockopt(sockSrv, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));  //允许地址复用

    memset(&addrSrv, 0, sizeof(struct sockaddr_in)); //addrSrv地址清零
    addrSrv.sin_family = AF_INET;  //ipv4
    addrSrv.sin_addr.s_addr = inet_addr("192.168.239.128");  //ip地址
    addrSrv.sin_port = htons(8000);  //8000端口

    if(-1 == bind(sockSrv, (struct sockaddr *)&addrSrv, sizeof(struct sockaddr)))  //socket绑定socket地址
    {
        std::cout << "服务器绑定失败：" << errno << "!" << std::endl;
        return -1;
    }

    serv_len = sizeof(struct sockaddr_in);
    getsockname(sockSrv, (struct sockaddr *)&serv, (socklen_t  *)&serv_len); //获得本地套接字地址
    std::cout << "服务器已经启动， ip地址为: " << inet_ntoa(serv.sin_addr) << " , 端口号为： " << ntohs(serv.sin_port) << std::endl;

    if(-1 == listen(sockSrv, 5))  //监听
    {
        std::cout << "服务器监听失败：" << errno << "!" << std::endl;
        return -1;
    }


    client_len = sizeof(struct sockaddr_in);
    struct MyData *mydata;
    int cn = 0;
    while(true)
    {
        std::cout << "----------等待客户端连接----------" << std::endl;
        int sockConn = accept(sockSrv, (struct sockaddr *)&addrClient, (socklen_t *)&client_len);//从连接请求队列取出客户端请求，如果为空则阻塞

        printf("----------客户端连接----------\n");

        cn = 5550; //要发送的字节数，由服务器端设定，客户端不知道
        mydata = (MyData *)malloc(sizeof(MyData) + cn);
        mydata->nLen = htonl(cn);
        memset(mydata->data, 'a', cn);
        mydata->data[cn-1] = 'b';
        send(sockConn, (char*) mydata, sizeof(MyData) + cn, 0); //发送给客户端

        free(mydata);
        
        char recvBuf[BUF_LEN];
        do{

            iRes = recv(sockConn, recvBuf, BUF_LEN, 0);
            if (iRes > 0)
            { 
                std::cout << "接收到了客户端的 " << iRes << " bytes" << std::endl;
                for(int i  = 0; i < iRes ; i++)
                {
                    printf("%c",recvBuf[i]);
                }
                printf("\n");
                memset(recvBuf, 0 , sizeof(recvBuf));
            }
            else if (iRes == 0)
            {
                std::cout << "客户端关闭了连接" << std::endl;
            }
            else{
                std::cout << "接收失败，错误代码为：" << errno << std::endl;
                close(sockConn);
                return 1;
            }
        }while (iRes > 0);

        
        close(sockConn);
        
        puts("服务器是否继续监听? (y/n): ");
        std::string ans;
        std::cin >> ans ;
        if (ans != "y") break;
    }

    close(sockSrv);

    return 0;
}