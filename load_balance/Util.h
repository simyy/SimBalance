#ifndef _UTIL_H_
#define _UTIL_H_

#include <WinSock2.h>
#include <map>
#include <string>
#include <iostream>
#include <set>
#include <queue>
#include <list>
#include <utility>
#include <Windows.h>
#include <signal.h>
#include <atlstr.h>
#include <sstream>
#include <fstream>
#include <time.h>
#include <stdlib.h>

using namespace std;

#include "tinyxml.h"
#include "tinystr.h"
#include "config.h"
#include "log.h"

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib,"tinyxml.lib") //加入链接库

//默认端口为8888 8889
#define M_PORT 8888


struct  t_msg{
	/* 消息的发送进程是谁，就填谁的id */
	unsigned src_id;
	/* 消息的接收进程是谁，就填谁的id */
	unsigned dst_id;
	/* 发送“时间请求”消息时填写，
	回复“时间应答”消息时，其值要与请求消息保持一致。 */
	unsigned usr_id;
	/* 消息类型：0, 时间请求；1, 时间答应；2, 心跳请求；3, 心跳应答 */
	unsigned msg_type;
	/* 服务端回复“时间应答”消息时，
	在data中填入当前时间的字符串，形式如“2013-06-20 13:56:28”即可  */
	char data[32];
};

//参数集合
struct M_params{
	//用于通信的套接字描述符
	SOCKET sock;
	//接收消息总数
	int *Num;
	//存储已连接的客户端/服务器
	map<int, sockaddr_in> *addr_map;
	//存储待处理的消息（向客户端/服务器发送的）
	queue<t_msg> *msg;
};

#endif