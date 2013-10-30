#include "Util.h"
#include "server.h"

CRITICAL_SECTION cs;//�����߳�ͬ��--�ٽ���

//ȫ�ֱ���,���ڿ���̨�����쳣�رյĻص�����
sockaddr_in *p;
SOCKET      *sock;
int         *mm_id;

Server::Server()
{
	server.sin_family = AF_INET;
	server.sin_port = htons(M_PORT+3);
	server.sin_addr.S_un.S_addr = INADDR_ANY;

	load_server.sin_family = AF_INET;
	load_server.sin_port = htons(M_PORT+1);
	load_server.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

	p = &load_server;

	id = 1;

	mm_id = &id;

	send_num = 0;
	recv_num = 0;
	send_error_num = 0;
	recv_error_num = 0;
}

Server::Server(const int port, const int m_id)
{
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.S_un.S_addr = INADDR_ANY;

	load_server.sin_family = AF_INET;
	load_server.sin_port = htons(M_PORT+1);
	load_server.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	
	p = &load_server;
	id = m_id;
	mm_id = &id;

	send_num = 0;
	recv_num = 0;
	send_error_num = 0;
	recv_error_num = 0;
}

Server::Server(const int port, const int m_id, const char* load_serv_addr)
{
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.S_un.S_addr = INADDR_ANY;

	load_server.sin_family = AF_INET;
	load_server.sin_port = htons(M_PORT+1);
	load_server.sin_addr.S_un.S_addr = inet_addr(load_serv_addr);

	p = &load_server;
	id = m_id;
	mm_id = &id;

	send_num = 0;
	recv_num = 0;
	send_error_num = 0;
	recv_error_num = 0;
}

Server::~Server(){
	 closesocket(m_socket);
	 WSACleanup();
	 DeleteCriticalSection(&cs);
}

void Server::init_server()
{
	//��ʼ��
	WSADATA wsaData;
	WORD sockVersion = MAKEWORD(2,2);
	if(WSAStartup(sockVersion, &wsaData) != 0){
		return;
	}
	//����socket
	m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(m_socket == INVALID_SOCKET){
		cout<<"socket error!"<<endl;
		return;
	}

	sock = &m_socket;

	int flag;
	//�󶨷�����
	flag = bind(m_socket, (sockaddr*)&server, sizeof(server));
	if(flag == SOCKET_ERROR){
		cout<<"bind error!"<<endl;
		return;
	}
	start_communication();
}

//�رտ���̨�����˳���������Ϣ
BOOL CtrlHandler(DWORD fdwCtrlType) 
{ 
	switch (fdwCtrlType) 
	{ 
		// Handle the CTRL+C signal. 

	case CTRL_C_EVENT: 

		Beep(1000, 1000); 
		return TRUE; 

		// CTRL+CLOSE: confirm that the user wants to exit. 

	case CTRL_CLOSE_EVENT: //���ؾ�������������˳���ʾ 
		{
			t_msg *msg = new t_msg;
			msg->dst_id = 0;
			msg->src_id = *mm_id;
			memcpy(msg->data, "quit", sizeof("quit"));
			int flag = sendto(*sock, (char*)msg, sizeof(t_msg), 0, (sockaddr*)p, sizeof(sockaddr_in));
			if(flag < 0)
				cout<<"sendto quit error!\n"<<endl;
			closesocket(*sock);
		}
		return TRUE; 

		// Pass other signals to the next handler. 

	case CTRL_BREAK_EVENT: 

	case CTRL_LOGOFF_EVENT: 

	case CTRL_SHUTDOWN_EVENT: 

	default: 

		return FALSE; 
	} 
} 

//���ڽ��ո��ؾ����������Ϣ���߳�
void Thread(M_params *p)
{
	sockaddr_in load_server;
	int lslen = sizeof(load_server);
	int flag;
	t_msg *msg = new t_msg;
	//UDP����ʽ����
	while(1){
		memset(msg, NULL, sizeof(t_msg));
		flag = recvfrom(p->sock, (char*)msg, sizeof(t_msg), 0, (sockaddr*)&load_server, &lslen);
		//���մ���+1
		++(*(p->recv_n));
		if(flag < 0){
			//���մ���+1
			++(*(p->recv_error_n));
			cout<<">>---->> recv error"<<endl<<endl;
			continue;
		}
		//ID����
		if(msg->dst_id != *mm_id){
			//���մ���+1
			++(*(p->recv_error_n));
			cout<<">>---->> recv a error msg"<<endl<<endl;
			continue;
		}
		cout<<">>---->> recv request from server"<<endl;
		cout<<"msg: "<<msg->data<<" User id:"<<msg->usr_id<<endl<<endl;
		EnterCriticalSection(&cs);
		//����Ϣ���뵽��Ϣ������
		p->q->push(*msg);
		LeaveCriticalSection(&cs);
	}
}

void Server::start_communication()
{
	InitializeCriticalSection(&cs);
	BOOL fSuccess; 
	//�򿪿���̨�����쳣�رջص�����
	fSuccess = SetConsoleCtrlHandler( 
		(PHANDLER_ROUTINE)CtrlHandler,  // handler function 
		TRUE);                           // add to list 
	if (! fSuccess) 
		cout<<"setConsoleCtrlHandler error"<<endl;

	//p1Ϊ�����߳���Ҫ�Ĳ���
	M_params *p1 = (M_params*)malloc(sizeof(M_params));
	p1->sock = m_socket;
	p1->q = &recv_client_msg;
	p1->recv_n = &recv_num;
	p1->recv_error_n = &recv_error_num;

	//�����߳�
	HANDLE hThread;
	DWORD ThreadID;
	hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Thread, p1, 0, &ThreadID);

	int flag;
	t_msg *msg = new t_msg;
	
	int load_len = sizeof(load_server);
	int i=1;

	while(1){
		memset(msg, NULL, sizeof(t_msg));
		if(i>0){
			cout<<"------------>>register<<-----------"<<endl;
			//����������,���ڸ��ؾ������ӱ���������ע����Ϣ
			msg->src_id = id;
			msg->dst_id = 0;
			strcpy(msg->data, "server");
			//����ע����Ϣ
			flag = sendto(m_socket, (char*)msg, sizeof(t_msg), 0, (sockaddr*)&load_server, sizeof(load_server));
			if(flag < 0){
				++send_error_num;
				cout<<"register error!"<<endl;
				continue;
			}
			i = 0;//i��1��0���Ժ��ٲ�������if��䣬��ֻע��һ��
			cout<<"------------>>start<<--------------"<<endl<<endl;
		}else{
			//�鿴��Ϣ�����Ƿ�Ϊ�գ�����Ϣ�Ļ����ظ�����Ϣ
			if(!recv_client_msg.empty()){
				//��ȡʱ��
				time_t t = time(0);
				strftime(msg->data, sizeof(msg->data), "%Y-%m-%d %H:%M:%S", localtime(&t)); 
				//�޸���ϢΪӦ����Ϣ
				msg->usr_id = recv_client_msg.front().usr_id;
				msg->dst_id = 0;
				msg->src_id = id;
				msg->msg_type = recv_client_msg.front().msg_type;
				//����Ӧ����Ϣ����������+1
				++send_num;
				flag = sendto(m_socket, (char*)msg, sizeof(t_msg), 0,(sockaddr*)&load_server, sizeof(load_server));
				if(flag < 0){
					cout<<"---->>send error!"<<endl;
					Sleep(2000);//�������,�ӳ�2����������
					continue;
				}
				EnterCriticalSection(&cs);
				if(!recv_client_msg.empty()){
					recv_client_msg.pop();
				}
				//��ʾͳ����Ϣ
				cout<<"         send response to server! >>---->>"<<endl; 
				cout<<"msg: "<<msg->data<<"User id: "<<msg->usr_id<<endl;
				cout<<"Send num: "<<send_num<<"  error num: "<<send_error_num<<endl;
				cout<<"Recv num: "<<recv_num<<"  error num: "<<recv_error_num<<endl;
				cout<<"---------------------------------------------"<<endl<<endl;
				LeaveCriticalSection(&cs);
			}
		}
	}
	delete msg;
}