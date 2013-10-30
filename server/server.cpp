#include "Util.h"
#include "server.h"

CRITICAL_SECTION cs;//用于线程同步--临界区

//全局变量,用于控制台窗口异常关闭的回调函数
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
	//初始化
	WSADATA wsaData;
	WORD sockVersion = MAKEWORD(2,2);
	if(WSAStartup(sockVersion, &wsaData) != 0){
		return;
	}
	//创建socket
	m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(m_socket == INVALID_SOCKET){
		cout<<"socket error!"<<endl;
		return;
	}

	sock = &m_socket;

	int flag;
	//绑定服务器
	flag = bind(m_socket, (sockaddr*)&server, sizeof(server));
	if(flag == SOCKET_ERROR){
		cout<<"bind error!"<<endl;
		return;
	}
	start_communication();
}

//关闭控制台发送退出服务器消息
BOOL CtrlHandler(DWORD fdwCtrlType) 
{ 
	switch (fdwCtrlType) 
	{ 
		// Handle the CTRL+C signal. 

	case CTRL_C_EVENT: 

		Beep(1000, 1000); 
		return TRUE; 

		// CTRL+CLOSE: confirm that the user wants to exit. 

	case CTRL_CLOSE_EVENT: //向负载均衡服务器发送退出提示 
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

//用于接收负载均衡服务器消息的线程
void Thread(M_params *p)
{
	sockaddr_in load_server;
	int lslen = sizeof(load_server);
	int flag;
	t_msg *msg = new t_msg;
	//UDP阻塞式接收
	while(1){
		memset(msg, NULL, sizeof(t_msg));
		flag = recvfrom(p->sock, (char*)msg, sizeof(t_msg), 0, (sockaddr*)&load_server, &lslen);
		//接收次数+1
		++(*(p->recv_n));
		if(flag < 0){
			//接收错误+1
			++(*(p->recv_error_n));
			cout<<">>---->> recv error"<<endl<<endl;
			continue;
		}
		//ID不符
		if(msg->dst_id != *mm_id){
			//接收错误+1
			++(*(p->recv_error_n));
			cout<<">>---->> recv a error msg"<<endl<<endl;
			continue;
		}
		cout<<">>---->> recv request from server"<<endl;
		cout<<"msg: "<<msg->data<<" User id:"<<msg->usr_id<<endl<<endl;
		EnterCriticalSection(&cs);
		//把消息加入到消息队列中
		p->q->push(*msg);
		LeaveCriticalSection(&cs);
	}
}

void Server::start_communication()
{
	InitializeCriticalSection(&cs);
	BOOL fSuccess; 
	//打开控制台窗口异常关闭回调函数
	fSuccess = SetConsoleCtrlHandler( 
		(PHANDLER_ROUTINE)CtrlHandler,  // handler function 
		TRUE);                           // add to list 
	if (! fSuccess) 
		cout<<"setConsoleCtrlHandler error"<<endl;

	//p1为接收线程需要的参数
	M_params *p1 = (M_params*)malloc(sizeof(M_params));
	p1->sock = m_socket;
	p1->q = &recv_client_msg;
	p1->recv_n = &recv_num;
	p1->recv_error_n = &recv_error_num;

	//创建线程
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
			//请求建立连接,并在负载均衡端添加本服务器的注册信息
			msg->src_id = id;
			msg->dst_id = 0;
			strcpy(msg->data, "server");
			//发送注册消息
			flag = sendto(m_socket, (char*)msg, sizeof(t_msg), 0, (sockaddr*)&load_server, sizeof(load_server));
			if(flag < 0){
				++send_error_num;
				cout<<"register error!"<<endl;
				continue;
			}
			i = 0;//i从1边0，以后再不会进入此if语句，故只注册一次
			cout<<"------------>>start<<--------------"<<endl<<endl;
		}else{
			//查看消息队列是否为空，有消息的话，回复次消息
			if(!recv_client_msg.empty()){
				//获取时间
				time_t t = time(0);
				strftime(msg->data, sizeof(msg->data), "%Y-%m-%d %H:%M:%S", localtime(&t)); 
				//修改消息为应答消息
				msg->usr_id = recv_client_msg.front().usr_id;
				msg->dst_id = 0;
				msg->src_id = id;
				msg->msg_type = recv_client_msg.front().msg_type;
				//发送应答消息，发送总数+1
				++send_num;
				flag = sendto(m_socket, (char*)msg, sizeof(t_msg), 0,(sockaddr*)&load_server, sizeof(load_server));
				if(flag < 0){
					cout<<"---->>send error!"<<endl;
					Sleep(2000);//如果出错,延迟2秒后继续发送
					continue;
				}
				EnterCriticalSection(&cs);
				if(!recv_client_msg.empty()){
					recv_client_msg.pop();
				}
				//显示统计信息
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