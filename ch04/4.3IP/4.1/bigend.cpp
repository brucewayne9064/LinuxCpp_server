#include <stdio.h>

int isBigEndian()
{
    unsigned short test = 0x1234; // 00010010 00110100
    if (*((unsigned char *)&test) == 0x12) // 取低地址处的字节
        return 1; // 大端模式
    else
        return 0; // 小端模式
}

int main()
{
    if (isBigEndian())
        printf("This machine is big endian.\n");
    else
        printf("This machine is little endian.\n");
    return 0;
}