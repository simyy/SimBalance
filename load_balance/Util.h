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
#pragma comment(lib,"tinyxml.lib") //�������ӿ�

//Ĭ�϶˿�Ϊ8888 8889
#define M_PORT 8888


struct  t_msg{
	/* ��Ϣ�ķ��ͽ�����˭������˭��id */
	unsigned src_id;
	/* ��Ϣ�Ľ��ս�����˭������˭��id */
	unsigned dst_id;
	/* ���͡�ʱ��������Ϣʱ��д��
	�ظ���ʱ��Ӧ����Ϣʱ����ֵҪ��������Ϣ����һ�¡� */
	unsigned usr_id;
	/* ��Ϣ���ͣ�0, ʱ������1, ʱ���Ӧ��2, ��������3, ����Ӧ�� */
	unsigned msg_type;
	/* ����˻ظ���ʱ��Ӧ����Ϣʱ��
	��data�����뵱ǰʱ����ַ�������ʽ�硰2013-06-20 13:56:28������  */
	char data[32];
};

//��������
struct M_params{
	//����ͨ�ŵ��׽���������
	SOCKET sock;
	//������Ϣ����
	int *Num;
	//�洢�����ӵĿͻ���/������
	map<int, sockaddr_in> *addr_map;
	//�洢���������Ϣ����ͻ���/���������͵ģ�
	queue<t_msg> *msg;
};

#endif