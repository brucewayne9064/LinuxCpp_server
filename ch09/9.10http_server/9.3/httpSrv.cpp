#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <ctype.h>
#include <sys/wait.h>

#define ISspace(x) isspace((int)(x))   //宏定义，它将ISspace(x)替换为isspace((int)(x))，其中isspace是一个C库函数，用于检查一个字符是否是空白字符
#define SERVER_STRING "Server: jdbhttpd/0.1.0\r\n"

int startup(u_short *);
void * accept_request(void *);
void execute_cgi(int, const char *, const char *, const char *);

void unimplemented(int); //501
void not_found(int); //404
void bad_request(int); //400
void cannot_execute(int); //500

void serve_file(int, const char *);
void headers(int, const char *); //200
void cat(int, FILE *);  //文件正文

void error_die(const char *);
int get_line(int, char *, int);



int main(int argc , char* argv[])  
{
	int server_sock = -1;
	u_short port = 8888;  //服务器监听端口


	int client_sock = -1;
	struct sockaddr_in client_addr;
	socklen_t client_addr_len = sizeof(client_addr);


	pthread_t newthread;

	//建立监听套接字，在对应端口建立http服务
	server_sock = startup(&port);
	printf("httpd running on port %d\n", port);
	
	
	while (1)
	{
	//阻塞等待客户端的连接请求
	client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_addr_len);
	if (client_sock == -1)
		error_die("accept error");

	//线程函数是accept_request，参数是client_sock，用newthread这个线程来处理client_sock这个新链接
	if (pthread_create(&newthread , NULL, accept_request, (void *)client_sock) != 0)
		perror("pthread_create error");
	}

	close(server_sock);
	return(0);
}


//监听套接字建立函数
/**********************************************************************/
/* This function starts the process of listening for web connections
 * on a specified port.  If the port is 0, then dynamically allocate a
 * port and modify the original port variable to reflect the actual
 * port.
 * Parameters: pointer to variable containing the port to connect on
 * Returns: the socket */
/**********************************************************************/
int startup(u_short *port)
{
	int httpd = 0;
	struct sockaddr_in server_addr;

	httpd = socket(PF_INET, SOCK_STREAM, 0);
	if (httpd == -1)
		error_die("socket error");

	
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(*port);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(httpd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
		error_die("bind error");


	//如果端口没有设置，随机分配一个端口, 把这个随机分配的端口赋值给port
	if (*port == 0)  
	{
		int addrlen = sizeof(server_addr);
		if (getsockname(httpd, (struct sockaddr *)&server_addr, (socklen_t *)&addrlen) == -1)
			error_die("getsockname error");
		*port = ntohs(server_addr.sin_port);
	}  
	if (listen(httpd, 5) == -1)
		error_die("listen error");
	return(httpd);
}


/**********************************************************************/
/* Execute a CGI script.  Will need to set environment variables as
 * appropriate.
 * Parameters: client socket descriptor
 *             path to the CGI script */
/**********************************************************************/
void execute_cgi(int client, const char *path, const char *method, const char *query_string)
{
 	char buf[1024];
 	int cgi_output[2];  //建立两个管道，每个管道都有读写两个端口,pipe(cgi_output),cgi_output[0]用于读数据，cgi_output[1]用于写数据
 	int cgi_input[2];
 	pid_t pid;
 	int status;
 	int i;
 	char c;
 	int numchars = 1;
 	int content_length = -1;

 	buf[0] = 'A'; buf[1] = '\0';

 	if (strcasecmp(method, "GET") == 0)
  		while ((numchars > 0) && strcmp("\n", buf))  //读取并丢弃请求头部
   			numchars = get_line(client, buf, sizeof(buf));
 	else    /* POST ，需要读取请求正文的长度，Content-Length长度为15*/
 	{
  		numchars = get_line(client, buf, sizeof(buf));

  		while ((numchars > 0) && strcmp("\n", buf))
  		{
   			buf[15] = '\0';
			/*
			这句代码的意思是，把缓冲区中从第16个字符开始，到遇到第一个非数字字符为止
			的字符串转换为整数，赋值给content_length变量。
			这是因为Content-Length字段的格式是这样的：Content-Length: 123
			所以从第16个字符开始，就是请求正文的长度。atoi函数是一个标准库函数，用于把字符串转换为整数。 
			 */
   			if (strcasecmp(buf, "Content-Length:") == 0)
    			content_length = atoi(&(buf[16]));

   			numchars = get_line(client, buf, sizeof(buf));
  		}
		//如果没有找到Content-Length字段，或者其值为-1，就说明请求格式有误，返回一个错误响应给客户端。
  		if (content_length == -1) {
   			bad_request(client);
   			return;
  		}		
 	}

 	sprintf(buf, "HTTP/1.0 200 OK\r\n");  //返回HTTP200成功报文的状态行
 	send(client, buf, strlen(buf), 0);

	//创建两个管道，pipe函数的返回值是一个整数，如果成功，返回0，如果失败，返回-1，并设置errno变量。
	/* 

	子进程
	x <------------------> 1  output
	0 <------------------> x  input
	父进程
	0 <------------------> x  output
	x <------------------> 1  input	

	 */
 	if (pipe(cgi_output) == -1) {
  		cannot_execute(client);
  		return;
 	}

 	if (pipe(cgi_input) == -1) {
  		cannot_execute(client);
  		return;
 	}

	//创建子进程
 	if ( (pid = fork()) == -1 ) {
  		cannot_execute(client);
  		return;
 	}

 	if (pid == 0)  // pid返回值为0，表示是子进程，如果大于0则表示运行的是父进程，pid值为子进程的pid
 	{
  		char meth_env[255];
  		char query_env[255];
  		char length_env[255];


		//STDIN和STDOUT的文件描述符分别是0和1
		//int dup2(int oldfd, int newfd);把指定的newfd指向oldfd指向的文件,成功返回newfd，失败返回-1.
  		dup2(cgi_output[1], 1);  //把STDOUT重定位到cgi_output[1]
  		dup2(cgi_input[0], 0);   //把STDIN重定位到cgi_input[0]
  		close(cgi_output[0]);    //关闭cgi_output[0]
  		close(cgi_input[1]);     //关闭cgi_input[1]

		
		//putenv配置cgi环境变量，可以用getenv获得这些变量
		//请求方法
  		sprintf(meth_env, "REQUEST_METHOD=%s", method);
  		putenv(meth_env);

		//如果是GET方法，就设置请求参数
  		if (strcasecmp(method, "GET") == 0) {
   			sprintf(query_env, "QUERY_STRING=%s", query_string);
   			putenv(query_env);
  		}
  		else {   //如果是post就设置请求正文长度
   			sprintf(length_env, "CONTENT_LENGTH=%d", content_length);
   			putenv(length_env);
  		}
		//执行函数，第一个是路径，第二个参数是文件名，最后需要用NULL结尾
  		execl(path, path, NULL);
  		exit(0);

 	} else {    //父进程
  		close(cgi_output[1]);  //关闭cgi_output[1]
  		close(cgi_input[0]);   //关闭cgi_input[0]
  		if (strcasecmp(method, "POST") == 0)
			//得到post的请求数据，写到input管道，供子进程使用
   			for (i = 0; i < content_length; i++) {
    			recv(client, &c, 1, 0);
    			write(cgi_input[1], &c, 1);
   			}
		//从output管道读到子进程处理后的信息，发给客户端
  		while (read(cgi_output[0], &c, 1) > 0)
   			send(client, &c, 1, 0);

		//关闭管道
  		close(cgi_output[0]);
  		close(cgi_input[1]);

		//等待子进程pid结束，并且把退出状态存在status
  		waitpid(pid, &status, 0);
 	}
}


/**********************************************************************/
/* A request has caused a call to accept() on the server port to
 * return.  Process the request appropriately.
 * Parameters: the socket connected to the client */
/**********************************************************************/
void * accept_request(void * arg)
{
	int client = (intptr_t)arg;

	char buf[1024];
	int numchars;
	char method[255];  //请求方法
	char url[255];     //url
	char path[512];    //网络资源的存放路径
	size_t i, j;
	struct stat st;
	int cgi = 0;
	char *query_string = NULL;

	numchars = get_line(client, buf, sizeof(buf));  //从client套接字中读取一行数据，存储到buf中，返回数据长度numchars
	i = 0; j = 0;
	//把客户端的请求方法存到method
	//Method(space)Request-URI(space)HTTP-Version\r\n
	//GET /index.html HTTP/1.1\r\n
	while (!ISspace(buf[j]) && (i < sizeof(method) - 1))
	{
		method[i] = buf[j];
		i++;
		j++;
	}
	method[i] = '\0';

	//strcasecmp返回非零值表示不相等，返回0表示相等
	//如果method既不是post也不是get，告诉客户端所请求的method不合法
	if (strcasecmp(method, "GET") && strcasecmp(method, "POST"))
	{
		unimplemented(client);
		close(client);
		return (void *) 0;
	}



	if (strcasecmp(method, "POST") == 0)
		cgi = 1;  //请求方法是post，说明需要向指定资源提交数据进行处理请求，所以需要调用cgi程序

	i = 0;
	while (ISspace(buf[j]) && (j < sizeof(buf)))  //跳过空格
	j++;
	//把客户端的URL存到url
	while (!ISspace(buf[j]) && (i < sizeof(url) - 1) && (j < sizeof(buf)))
	{
		url[i] = buf[j];
		i++; 
		j++;
	}
	url[i] = '\0';

	//请求方法是get，说明只是想特定资源发送请求，需要获取请求参数
	if (strcasecmp(method, "GET") == 0)
	{
		query_string = url;
		while ((*query_string != '?') && (*query_string != '\0'))
			query_string++;
		if (*query_string == '?') //如果URL里有参数，那么在问号后，那么也需要执行cgi程序
		{
			cgi = 1;
			*query_string = '\0';  //把问号替换成\0，相当于截断URL，query_string指针指向参数，而url里则没有了
			query_string++;  //query_string指向URL参数的第一个字节
		}
	}

 	sprintf(path, "htdocs%s", url);  //构造网络资源的存放路径./htdocs/url，存放到path
 	if (path[strlen(path) - 1] == '/')  //path是一个目录，且以/结尾， 但是有可能出现是目录但是没有以/结尾的情况，这时需要后面的第二次拼接
		strcat(path, "index.html");  //默认访问index.html
 	if (stat(path, &st) == -1) {     //获取path指定的文件或目录的状态信息，比如大小，类型，权限，修改时间等，并将这些信息存储在st指向的结构体中
		while ((numchars > 0) && strcmp("\n", buf))  
	 		numchars = get_line(client, buf, sizeof(buf));  //读取并丢弃请求头部
		not_found(client);  //向客户端发送HTTP404
 	}
 	else
 	{
		if ((st.st_mode & S_IFMT) == S_IFDIR)  //如果st.st_mode是一个目录类型，path的最后一个字符不是'/'
			strcat(path, "/index.html");
		if ((st.st_mode & S_IXUSR) || (st.st_mode & S_IXGRP) || (st.st_mode & S_IXOTH))  //判断执行权限，但是前面post方法和带参数get都开启了cgi，这里是不是应该加上没权限就改成0？
	 		cgi = 1;
		else{
			cgi = 0;
		}
		if (!cgi)  //如果不需要执行cgi程序，那么直接返回所请求的文件
	 		serve_file(client, path);
		else   //执行cgi文件
	 		execute_cgi(client, path, method, query_string);  
	}

	close(client);

	return (void*) 0;
}


//错误处理函数
/**********************************************************************/
/* Print out an error message with perror() (for system errors; based
 * on value of errno, which indicates system call errors) and exit the
 * program indicating an error. */
/**********************************************************************/
void error_die(const char *sc)
{
	perror(sc);
	exit(1);
}


/**********************************************************************/
/* Send a regular file to the client.  Use headers, and report
 * errors to client if they occur.
 * Parameters: a pointer to a file structure produced from the socket
 *              file descriptor
 *             the name of the file to serve */
/**********************************************************************/
void serve_file(int client, const char *filename)
{
 	FILE *resource = NULL;
 	int numchars = 1;
 	char buf[1024];

 	buf[0] = 'A'; buf[1] = '\0';  //用任意一个非空字符来初始化buf，让buf不是一个空指针，避免在进行strcmp时出错
 	while ((numchars > 0) && strcmp("\n", buf))  //读取并丢弃请求头部
  		numchars = get_line(client, buf, sizeof(buf));

 	resource = fopen(filename, "r");
 	if (resource == NULL)
  		not_found(client);
 	else
 	{
  		headers(client, filename);  //给客户端发送http200报头
  		cat(client, resource);  //给客户端发送文件内容
 	}
 	fclose(resource);
}



//从一个套接字中读取一行数据，存储在一个字符数组中，并返回读取的字符数。
/**********************************************************************/
/* Get a line from a socket, whether the line ends in a newline,
 * carriage return, or a CRLF combination.  Terminates the string read
 * with a null character.  If no newline indicator is found before the
 * end of the buffer, the string is terminated with a null.  If any of
 * the above three line terminators is read, the last character of the
 * string will be a linefeed and the string will be terminated with a
 * null character.
 * Parameters: the socket descriptor
 *             the buffer to save the data in
 *             the size of the buffer
 * Returns: the number of bytes stored (excluding null) */
/**********************************************************************/
int get_line(int sock, char *buf, int size)
{
	int i = 0;  //字符数组下标
	char c = '\0';  //接收缓冲区，\0是字符串结束的标志
	int n;   //接收函数返回值，表示收到的字节数

	while ((i < size - 1) && (c != '\n'))  //未越界，未遇到换行符 循环
	{
		n = recv(sock, &c, 1, 0);  //接收一个字节
		if (n > 0)
		{
			//第一行的结尾只有三种情况，分别是\r\n和\n和\r，如果读到\r就看下一个是不是\n，如果是就接收到c里，如果不是也把c换成\n
			if (c == '\r')  //回车
			{
				n = recv(sock, &c, 1, MSG_PEEK);  //预览下一字节
				if ((n > 0) && (c == '\n'))
					recv(sock, &c, 1, 0);
				else
					c = '\n';
			}
			buf[i] = c;
			i++;
		}
		else
			c = '\n';
	}
	buf[i] = '\0';
 
	return(i);
}

//把文件一行一行发给客户端
/**********************************************************************/
/* Put the entire contents of a file out on a socket.  This function
 * is named after the UNIX "cat" command, because it might have been
 * easier just to do something like pipe, fork, and exec("cat").
 * Parameters: the client socket descriptor
 *             FILE pointer for the file to cat */
/**********************************************************************/
void cat(int client, FILE *resource)
{
 	char buf[1024];

 	fgets(buf, sizeof(buf), resource);
 	while (!feof(resource))
 	{
  		send(client, buf, strlen(buf), 0);
  		fgets(buf, sizeof(buf), resource);
 	}
}


//发送HTTP501错误报文
/**********************************************************************/
/* Inform the client that the requested web method has not been
 * implemented.
 * Parameter: the client socket */
/**********************************************************************/
void unimplemented(int client)
{
	char buf[1024];

	sprintf(buf, "HTTP/1.0 501 Method Not Implemented\r\n"); // 状态行
	send(client, buf, strlen(buf), 0);
	sprintf(buf, SERVER_STRING);                   //消息报头
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "Content-Type: text/html\r\n");   //消息报头
	send(client, buf, strlen(buf), 0);             
	sprintf(buf, "\r\n");                //空行
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "<HTML><HEAD><TITLE>Method Not Implemented\r\n");   //响应正文
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "</TITLE></HEAD>\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "<BODY><P>HTTP request method not supported.\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "</BODY></HTML>\r\n");
	send(client, buf, strlen(buf), 0);
}

//发送HTTP500错误报文
/**********************************************************************/
/* Inform the client that a CGI script could not be executed.
 * Parameter: the client socket descriptor. */
/**********************************************************************/
void cannot_execute(int client)
{
 	char buf[1024];

 	sprintf(buf, "HTTP/1.0 500 Internal Server Error\r\n");  //状态行
 	send(client, buf, strlen(buf), 0);
 	sprintf(buf, "Content-type: text/html\r\n");    //消息报头
 	send(client, buf, strlen(buf), 0);
 	sprintf(buf, "\r\n");                        //空行
 	send(client, buf, strlen(buf), 0);
 	sprintf(buf, "<P>Error prohibited CGI execution.\r\n");  //响应正文
 	send(client, buf, strlen(buf), 0);
}


//发送HTTP404错误报文
/**********************************************************************/
/* Give a client a 404 not found status message. */
/**********************************************************************/
void not_found(int client)
{
 	char buf[1024];

 	sprintf(buf, "HTTP/1.0 404 NOT FOUND\r\n");  //状态行
 	send(client, buf, strlen(buf), 0);
 	sprintf(buf, SERVER_STRING);                    //消息报头
 	send(client, buf, strlen(buf), 0);
 	sprintf(buf, "Content-Type: text/html\r\n");    //消息报头
 	send(client, buf, strlen(buf), 0);
 	sprintf(buf, "\r\n");                           //空行
 	send(client, buf, strlen(buf), 0);
 	sprintf(buf, "<HTML><TITLE>Not Found</TITLE>\r\n");   //响应正文
 	send(client, buf, strlen(buf), 0);
 	sprintf(buf, "<BODY><P>The server could not fulfill\r\n");
 	send(client, buf, strlen(buf), 0);
 	sprintf(buf, "your request because the resource specified\r\n");
 	send(client, buf, strlen(buf), 0);
 	sprintf(buf, "is unavailable or nonexistent.\r\n");
 	send(client, buf, strlen(buf), 0);
 	sprintf(buf, "</BODY></HTML>\r\n");
 	send(client, buf, strlen(buf), 0);
}



//发送HTTP400错误报文
/**********************************************************************/
/* Inform the client that a request it has made has a problem.
 * Parameters: client socket */
/**********************************************************************/
void bad_request(int client)
{
 	char buf[1024];

 	sprintf(buf, "HTTP/1.0 400 BAD REQUEST\r\n");   //状态行
 	send(client, buf, sizeof(buf), 0);
 	sprintf(buf, "Content-type: text/html\r\n");   //消息报头
 	send(client, buf, sizeof(buf), 0);
 	sprintf(buf, "\r\n");                          //空行
 	send(client, buf, sizeof(buf), 0);
 	sprintf(buf, "<P>Your browser sent a bad request, ");   //响应正文
 	send(client, buf, sizeof(buf), 0);
 	sprintf(buf, "such as a POST without a Content-Length.\r\n");
 	send(client, buf, sizeof(buf), 0);
}



//发送HTTP200成功报文 前三部分
/**********************************************************************/
/* Return the informational HTTP headers about a file. */
/* Parameters: the socket to print the headers on
 *             the name of the file */
/**********************************************************************/
void headers(int client, const char *filename)
{
 	char buf[1024];
 	const char *ext; //文件名的后缀
 	char *type; //文件的类型

 	ext = strrchr(filename, '.'); //找到文件名中最后一个点的位置
 	if (ext == NULL) //如果没有点，就默认为text/plain类型
 		type = "text/plain";
 	else if (strcmp(ext, ".html") == 0 || strcmp(ext, ".htm") == 0) //如果是.html或.htm，就是text/html类型
 		type = "text/html";
 	else if (strcmp(ext, ".jpg") == 0 || strcmp(ext, ".jpeg") == 0) //如果是.jpg或.jpeg，就是image/jpeg类型
 		type = "image/jpeg";
 	else if (strcmp(ext, ".png") == 0) //如果是.png，就是image/png类型
 		type = "image/png";
 	else if (strcmp(ext, ".gif") == 0) //如果是.gif，就是image/gif类型
 		type = "image/gif";
 	else if (strcmp(ext, ".css") == 0) //如果是.css，就是text/css类型
 		type = "text/css";
 	else if (strcmp(ext, ".js") == 0) //如果是.js，就是application/javascript类型
 		type = "application/javascript";
 	else //其他情况，就默认为text/plain类型
 		type = "text/plain";

 	strcpy(buf, "HTTP/1.0 200 OK\r\n");  //状态行
 	send(client, buf, strlen(buf), 0);
 	strcpy(buf, SERVER_STRING);          //消息报头
 	send(client, buf, strlen(buf), 0);
 	sprintf(buf, "Content-Type: %s\r\n", type);   //消息报头，根据type发送正确的Content-Type
 	send(client, buf, strlen(buf), 0);
 	strcpy(buf, "\r\n");                       //空行
 	send(client, buf, strlen(buf), 0);    
}