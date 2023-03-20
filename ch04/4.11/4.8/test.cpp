#include <cstdio>
#include <iostream>
#include<string.h>
using namespace std;
struct MyData
{
	int nLen;
	char data[0];
};
int main()
{
	int nLen = 10;
	char str[10] = "123456789";//别忘记还有一个'\0'，所以是10个字符哦。
    cout << "Size of str: " << sizeof(str) << endl; 

	cout << "Size of MyData: " << sizeof(MyData) << endl;  
	MyData *myData = (MyData*)malloc(sizeof(MyData) + 10);
	memcpy(myData->data, str, 10);
    cout << "Size of myData->nLen: " << sizeof(myData->nLen) << endl; 
    cout << "Size of myData->data: " << sizeof(myData->data) << endl; 
	cout << "myData's Data is: " << myData->data << endl;
	cout << "Size of MyData: " << sizeof(MyData) << endl;
	free(myData);
	getchar();

	return 0;
}



/* 
sizeof(myData->data)为什么这个大小为0的原因是：
myData->data是一个char data[0]的数组，它没有元素，所以它不占用结构体的空间，也就是说sizeof(char data[0])等于0。
sizeof运算符只能求得静态分配内存的数组的长度，而不能求得动态分配内存的数组的长度。myData->data是一个动态分配内存的数组，它的长度由malloc函数决定，而不是由sizeof函数决定。
sizeof运算符在编译时就已经确定了结果，而不会在运行时计算。myData->data是一个动态分配内存的数组，它的长度在运行时才能确定，所以sizeof函数无法获取它的真实长度。 
*/