#define TEST
#include "XFtpRETR.h"
#include "testUtil.h"
#include <event2/bufferevent.h>
#include <event2/event.h>
#include <iostream>
#include <string>

using namespace std;

void XFtpRETR::Parse(string type, string msg) {
	testout("At XFtpRETR::Parse");
	int pos = msg.rfind(" ") + 1;  //文件名开始的位置(最后一个空格后面的部分)
	string filename = msg.substr(pos, msg.size() - pos - 2);  //提取文件名的字符串
	string path = cmdTask->rootDir + cmdTask->curDir + filename;  //拼接完整路径
	testout("filepath:[" << path << "]");
	fp = fopen(path.c_str(), "rb");  //以二进制模式打开文件
	if (fp) {
		ConnectoPORT();
		ResCMD("150 File OK");
		bufferevent_trigger(bev, EV_WRITE, 0);
	}
	else {
		ResCMD("450 file open failed!");
	}
}

void XFtpRETR::Write(bufferevent *bev) {
	testout("At XFtpRETR::Write");
	if (!fp) return;
	int len = fread(buf, 1, sizeof(buf), fp);
	if (len <= 0) {
		ResCMD("226 Transfer complete");
		ClosePORT();
		return;
	}
	cout << buf;
	Send(buf, sizeof(buf));
}

void XFtpRETR::Event(bufferevent *bev, short events) {
	testout("At XFtpRETR::Event");
	if (events & (BEV_EVENT_EOF | BEV_EVENT_ERROR | BEV_EVENT_TIMEOUT)) {
		ClosePORT();
	}
	else if (events & BEV_EVENT_CONNECTED) {
		cout << "XFtpRETR BEV_EVENT_CONNECTED" << endl;
	}
}




