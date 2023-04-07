#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstdio>
#include <cstdlib>
#include <errno.h>
#include <cstring>
#include <initializer_list>
using std::initializer_list;
#include <vector> // 每个stl都需要对应的头文件 
using std::vector;


void errExit()  //错误处理函数
{
	getchar();
	exit(-1);
}

const char resp[] = "HTTP/1.1 200\r\n\
Content-Type: application/json\r\n\
Content-Length: 13\r\n\
Date: Thu, 2 Aug 2021 04:02:00 GMT\r\n\
Keep-Alive: timeout=60\r\n\
Connection: keep-alive\r\n\r\n\
[HELLO WORLD]\r\n\r\n";

int main (int argc, char* argv[]) {
	//创建套接字
	const int port = 8888;
	int sd, ret;
	sd = socket(AF_INET, SOCK_STREAM, 0);  // tcp fd
	fprintf(stderr, "created socket\n");
	if (sd == -1)
		errExit();
	int opt = 1;


	// 重用地址
	if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) == -1)
		errExit();
	fprintf(stderr, "socket opt set\n");


    //bind
	sockaddr_in addr;
	addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
	addr.sin_addr.s_addr = INADDR_ANY;
	socklen_t addrLen = sizeof(addr);
	if (bind(sd, (sockaddr *)&addr, sizeof(addr)) == -1)
		errExit();
	fprintf(stderr, "socket binded\n");


    //listen
	if (listen(sd, 1024) == -1)
		errExit();
	fprintf(stderr, "socket listen start\n");
	//套接字创建完毕
	//初始化监听列表
	// number of poll fds
	int currentFdNum = 1;
	pollfd *fds = static_cast<pollfd *>(calloc(100, sizeof(pollfd)));  //分配100个pollfd的空间
	fds[0].fd = sd, fds[0].events = POLLIN;   //把第一个设置为监听fd，event设置为读（pollin）
	nfds_t nfds = 1;  //指定fds里的pollfd的个数
	int timeout = -1;  //永远等待
    fprintf(stderr, "polling\n");



	while (1) {
		//执行poll操作
		ret = poll(fds, nfds, timeout);
		fprintf(stderr, "poll returned with ret value: %d\n", ret);
		if (ret == -1)  //poll failed
			errExit();
		else if (ret == 0) {   //poll超时前无事发生
			fprintf(stderr, "return no data\n");
		}
		else { // ret > 0，表示ret为revents中不为0的个数
		 // got accept
			fprintf(stderr, "checking fds\n");
			//检查是否有新客户端建立连接
			if (fds[0].revents & POLLIN) {  //情况1：客户端请求建立连接
				sockaddr_in childAddr;
				socklen_t childAddrLen;
				int childSd = accept(sd, (sockaddr *)&childAddr, &(childAddrLen));
				if (childSd == -1)
					errExit();
				fprintf(stderr, "child got\n");
				// set non_block
				int flags = fcntl(childSd, F_GETFL);  //取得childsd的标志状态
				// accept并设置为非阻塞
				if (fcntl(childSd, F_SETFL, flags | O_NONBLOCK) == -1)
					errExit();
				fprintf(stderr, "child set nonblock\n");
				// add child to list
				//假如到poll的描述符集，关心POLLIN事件
				fds[currentFdNum].fd = childSd;
                fds[currentFdNum].events = (POLLIN | POLLRDHUP);  //同时检查套接字是否有数据可读和是否已经断开连接(调用shutdown(SHUT_WR))
				nfds++, currentFdNum++; 
				fprintf(stderr, "child: %d pushed to poll list\n", currentFdNum - 1);
			}
			// child read & write
			//检查其他描述符的事件
			for (int i = 1; i < currentFdNum; i++) {
				if (fds[i].revents & (POLLHUP | POLLRDHUP | POLLNVAL)) { //同时检查套接字是否挂起，是否已经断开连接(调用shutdown(SHUT_WR))，是否文件描述符无效
					//客户端描述符关闭
					//设置events=0, fd=-1，不再关心
					// set not interested
					fprintf(stderr, "child: %d shutdown\n", i);
					close(fds[i].fd);
					fds[i].events = 0;
					fds[i].fd = -1;
					continue;
				}
				//  read
				if (fds[i].revents & POLLIN) {  //检查返回的revents是否为读
					char buffer[1024] = {};
					while (1) {
						//读取请求数据
						ret = read(fds[i].fd, buffer, 1024);
						fprintf(stderr, "read on: %d returned with value: %d\n", i, ret);
						if (ret == 0) {  //连接断开
						fprintf(stderr, "read returned 0(EOF) on: %d, breaking\n", i);  //连接已断开
							break;
						}
						if (ret == -1) {  // 发生错误
							const int tmpErrno = errno;

							if (tmpErrno == EWOULDBLOCK || tmpErrno == EAGAIN) {  //读取阻塞
								fprintf(stderr, "read would block, stop reading\n");
								fds[i].events |= POLLOUT;  //同时监听读和写，可以在写入数据时继续监听读事件
								break;
							}
							else {
								errExit();  
							}
						}
					}
				}
				// write
				if (fds[i].revents & POLLOUT) {  //检查返回的revents是否为写
					//写事件，把请求返回
					ret = write(fds[i].fd, resp, sizeof(resp));
					fprintf(stderr, "write on: %d returned with value: %d\n", i, ret);
					//这里需要处理 EAGAIN EWOULDBLOCK
					if (ret == -1) {
						errExit();
					}
					fds[i].events &= !(POLLOUT);  //取消写事件
				}
			}
		}
	}
	return 0;
}
 