#ifndef _UTIL_H_
#define _UTIL_H_

#include <WinSock2.h>
#include <map>
#include <string>
#include <iostream>
#include <queue>
#include <signal.h>

using namespace std;

#pragma comment(lib, "Ws2_32.lib")

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

#endif
