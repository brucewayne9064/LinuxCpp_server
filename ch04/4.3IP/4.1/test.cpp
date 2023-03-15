#include <cstdio>
#include <arpa/inet.h>
#include <iostream>
#include <type_traits>
int main()
{
	struct in_addr ia;

	std::cout << std::boolalpha;
	std::cout << "in_addr_t占用的字节数为：" << sizeof(in_addr_t) << std::endl;
	std::cout << "in_addr_t is signed: " << std::is_signed<in_addr_t>::value << '\n';
	std::cout << "in_addr_t是32位无符号整型"<<std::endl;
	std::cout << "输入ip地址172.16.2.6" << std::endl;
	in_addr_t dwIP = inet_addr("172.16.2.6"); //点分十进制转换为二进制，格式为in_addr_t（32位无符号整型）

	std::cout <<"网络字节序的十进制整数："<< dwIP << std::endl;
	ia.s_addr = dwIP;  //in_addr结构体里的s_addr是in_addr_t格式
	printf("网络字节序的16进制 ia.s_addr=0x%x\n", ia.s_addr);//172.16.2.6 = 0xac 0x10 0x02 0x06,打印结果是大端（网络字节序）
	printf("本地字节序的16进制%x\n", ntohl(dwIP)); // 打印本地字节序

	printf("real_ip=%s\n", inet_ntoa(ia));  //in_addr格式转点分十进制字符串
	return 0;
}