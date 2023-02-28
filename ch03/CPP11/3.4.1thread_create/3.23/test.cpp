#include<iostream>
#include<thread>

void thfunc (char *s)
{
    std::cout << "thfunc: " << s << std::endl;
}

int main(int argc, char *argv[])
{
    char s[] = "这是字符串";
    std::thread t(thfunc, s);
    t.join();
    return 0;
}