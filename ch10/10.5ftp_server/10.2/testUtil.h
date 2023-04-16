#pragma once

#include <iostream>

//如果TEST被定义了，那么就执行下面的代码，直到遇到#endif指令为止
#ifdef TEST  
//如果TEST被定义了，那么testout函数就被定义为一个宏，它接受一个参数msg，
//并将其输出到标准输出流cout中，并在末尾加上换行符和刷新操作5。
#define testout(msg) std::cout << msg << std::endl << std::flush
//#else指令表示如果#ifdef TEST的条件不满足，也就是TEST没有被定义，
//那么就执行#else后面的代码，直到遇到#endif指令为止
#else 
//如果TEST没有被定义，那么testout函数就被定义为一个空白的宏，
//它接受一个参数msg，但不做任何操作
#define testout(msg) 
//#endif指令表示结束#ifdef TEST和#else之间的条件编译区域
#endif


