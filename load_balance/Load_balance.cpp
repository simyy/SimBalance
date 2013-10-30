#include "Util.h"
#include "Load_balance.h"

#define SIO_UDP_CONNRESET _WSAIOW(IOC_VENDOR,12)

CRITICAL_SECTION cs;//�����߳�ͬ��--�ٽ���

//���ڼ�¼�������洢��˳��������ѯ
queue<int> server_q;

string conf_name = "conf.xml";

//������Ϣ����
int to_client_num = 0;
int to_server_num = 0;

//������Ϣ������
int to_client_error_num = 0;
int to_server_error_num = 0;

//������Ϣ������
int recv_client_error_num = 0;
int recv_server_error_num = 0;

int m_switch = 0;
/*
0: Close all
1: Open  detail message
2: Open  statistic information
3: Open  detail/statistic information
*/

//�źŴ���ص�����
void OnCtrlC(int)
{
	++m_switch;
	switch(m_switch){
	case 1:
		cout<<"    <<<Open  detail message>>>"<<endl<<endl;
		break;
	case 2:
		cout<<"    <<<Open  statistic infor>>>"<<endl<<endl;
		break;
	case 3:
		cout<<"    <<<Open  statistic/datail infor>>>"<<endl<<endl;
		break;
	default:
		cout<<"    <<<Close All infor>>>"<<endl<<endl;
		m_switch = 0;
		break;
	}
}

//Ĭ�Ϲ��캯�� 
Load_server::Load_server()
{
	local1.sin_family = AF_INET;
	local1.sin_port = htons(M_PORT);
	local1.sin_addr.S_un.S_addr = INADDR_ANY;

	local2.sin_family = AF_INET;
	local2.sin_port = htons(M_PORT+1);
	local2.sin_addr.S_un.S_addr = INADDR_ANY;

	m_socket1 = -1;
	m_socket2 = -1;

	recv_client_num = 1;
	recv_server_num = 1;
}

Load_server::Load_server(const int port)
{
	local1.sin_family = AF_INET;
	local1.sin_port = htons(port);
	local1.sin_addr.S_un.S_addr = INADDR_ANY;

	local2.sin_family = AF_INET;
	local2.sin_port = htons(port+1);
	local2.sin_addr.S_un.S_addr = INADDR_ANY;

	m_socket1 = -1;
	m_socket2 = -1;

	recv_client_num = 1;
	recv_server_num = 1;
}

//�������������ڹر��׽��ֵ�
Load_server::~Load_server()
{
	closesocket(m_socket1);
	closesocket(m_socket2);
	WSACleanup();
	DeleteCriticalSection(&cs);
}

//�����׽��ֲ���ɶ˿ڰ�
void Load_server::init_server()
{
	cout<<"******************************************"<<endl;
	cout<<"This is a Load balancing Server��"<<endl;
	cout<<"default id : 0"<<endl;
	cout<<"Ctrl+c : select detail/statistic mode or not"<<endl;
	cout<<"waiting for the message....."<<endl;
	cout<<"******************************************"<<endl;
	cout<<endl;

	//��ʼ��
	WSADATA wsaData;
	WORD sockVersion = MAKEWORD(2,2);
	if(WSAStartup(sockVersion, &wsaData) != 0){
		cout<<"WSAStartup error!"<<endl;
		string error = "WSAStartup error!";
		log_write(error);
        return;
	}

	//����socket����ͻ��ˡ����������ӵ�UDP��
	m_socket1 = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	m_socket2 = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(m_socket1 == INVALID_SOCKET){
		int i = WSAGetLastError();
		cout<<"socket1 create error! "<<i<<endl;
		char buf[5];
		itoa(i, buf, 10);
		string error = "socket1 create error! ";
		error.append(buf);
		log_write(error);
		return;
	}
	if(m_socket2 == INVALID_SOCKET){
		int i = WSAGetLastError();
		cout<<"socket2 create error! "<<i<<endl;
		char buf[5];
		itoa(i, buf, 10);
		string error = "socket2 create error! ";
		error.append(buf);
		log_write(error);
		return;
	}

	//�󶨶˿�
	int flag;
	flag = bind(m_socket1, (sockaddr*)&local1, sizeof(local1));
	if(flag == SOCKET_ERROR){
		int i = WSAGetLastError();
		cout<<"bind local1 error! "<<i<<endl;
		char buf[5];
		itoa(i, buf, 10);
		string error = "bind local1 error! ";
		error.append(buf);
		log_write(error);
		return;
	}
	flag = bind(m_socket2, (sockaddr*)&local2, sizeof(local2));
	if(flag == SOCKET_ERROR){
		int i = WSAGetLastError();
		cout<<"bind local2 error! "<<i<<endl;
		char buf[5];
		itoa(i, buf, 10);
		string error = "bind local2 error! ";
		error.append(buf);
		log_write(error);
		return;
	}

	start_communication();
}

//�������߳̽��շ�����Ӧ����Ϣ
void Thread1(M_params *p)
{
	int flag;
	sockaddr_in server;
	int fromlen = sizeof(server);
	t_msg *msg = new t_msg;
	p->addr_map->clear();

	fd_set  readfds;
	struct timeval  authtime;

	//���ó�ʱʱ��
	authtime.tv_usec = 0L;
	authtime.tv_sec = (long)1; // 1����ط�

	while(1){
		FD_ZERO (&readfds);
		FD_SET (p->sock, &readfds);
		//����select����
		if(select(p->sock, &readfds, NULL, NULL, &authtime)< 0){
			cout<<"select error!"<<endl;
			string error = "select error!";
			log_write(error);;
			continue;
		}

		if (FD_ISSET(p->sock, &readfds)) // �ж��������Ͷ�
		{

			//�������Է�������Ϣ
			memset(msg, NULL, sizeof(t_msg));
			memset(&server, NULL, sizeof(server));
			flag = recvfrom(p->sock, (char*)msg, sizeof(t_msg), 0, (sockaddr*)&(server), &fromlen);
			(*(p->Num))++;
			if(flag < 0){
				++recv_server_error_num;
				int i = WSAGetLastError();
				cout<<"recvfrom server error! "<<i<<endl;
				char buf[5];
				itoa(i, buf, 10);
				string error = "recvfrom server error! ";
				error.append(buf);
				EnterCriticalSection(&cs);
				log_write(error);
				LeaveCriticalSection(&cs);
				continue;
			}
			//���Ŀ�ĵص�ID�뱾�ز�ͬ��ɾ��
			if(msg->dst_id != 0){
				string str = "recv a error msg!";
				EnterCriticalSection(&cs);
				log_write(str);
				LeaveCriticalSection(&cs);
				++recv_server_error_num;
				continue;
			}

			EnterCriticalSection(&cs);
			//����Ƿ�����ע������
			if(!strcmp(msg->data,"server")){
				int i = msg->src_id;
				char buf[5];
				itoa(i, buf, 10);
				string str = "server ��";
				str.append(buf);
				str.append("�� register!");
				log_write(str);
				memset(msg->data, NULL, sizeof(msg->data));
				cout<<"---------------------------------------------"<<endl;
				cout<<"Register a new server.....Server id: "<<msg->src_id<<endl;
				//server��ַ���뵽serverĿ¼��
				server_q.push(msg->src_id);
				p->addr_map->insert(pair<int, sockaddr_in>(msg->src_id, server));
				cout<<"The number of servers : "<<p->addr_map->size()<<endl;
				cout<<"---------------------------------------------"<<endl;
				int id = msg->src_id;
				//�Ѹ÷�������ӵ������ļ���
				AddXmlFile(conf_name, id, server);
				//LeaveCriticalSection(&cs);
			}else if(!strcmp(msg->data, "quit")){
				//������˳�����
				int i = msg->src_id;
				char buf[5];
				itoa(i, buf, 10);
				string str = "server ��";
				str.append(buf);
				str.append("�� quit!");
				log_write(str);
				cout<<"---------------------------------------------"<<endl;
				cout<<"Server "<<msg->src_id<<" is closed!"<<endl;
				map<int, sockaddr_in>::iterator x;
				x = p->addr_map->find(msg->src_id);
				//�ӷ������б���ɾ���÷�������Ϣ
				if(x != p->addr_map->end())
					p->addr_map->erase(x);
				cout<<"Now the number of servers : "<<p->addr_map->size()<<endl;
				cout<<"---------------------------------------------"<<endl;
				int id = msg->src_id;
				//�������ļ���ɾ����������Ϣ
				//DeleteXmlFile(conf_name, id, server);
			}
			//���ص�Ӧ����Ϣ
			else{
				string str = "recv msg from server��";
				char buf[5];
				itoa(msg->src_id, buf, 10);
				str.append(buf).append("��!");
				log_write(str);
				//EnterCriticalSection(&cs);
				cout<<">>---->> recv a response from Server ��"<<msg->src_id<<"��"<<endl;
				if( m_switch == 1 || m_switch == 3){
					cout<<"-->message content: "<<msg->data<<endl;
				}
				cout<<endl;
				//�޸���Ϣ��ַ��Ϣ�����뵽��Ϣ������
				msg->dst_id = msg->usr_id;
				msg->src_id = 0;
				p->msg->push(*msg);
			}
			LeaveCriticalSection(&cs);
		}
	}
	delete msg;
}

//�ͻ����߳̽��տͻ�������Ϣ
void Thread2(M_params *p)
{
	int flag;
	sockaddr_in client;
	int clientlen = sizeof(client);
	t_msg *msg = new t_msg;
	p->addr_map->clear();

	fd_set  readfds;
	struct timeval  authtime;

	// ���ó�ʱʱ��
	authtime.tv_usec = 0L;
	authtime.tv_sec = (long)1; // 1����ط�

	while(1){
		FD_ZERO (&readfds);
		FD_SET (p->sock, &readfds);
		//����select����
		if(select(p->sock, &readfds, NULL, NULL, &authtime)< 0){
			cout<<"select error!"<<endl;
			string error = "select error!";
			EnterCriticalSection(&cs);
			log_write(error);
			LeaveCriticalSection(&cs);
			continue;
		}

		if (FD_ISSET(p->sock, &readfds)) // �ж��������Ͷ�
		{
			memset(msg, NULL, sizeof(t_msg));
			memset(&client, NULL, sizeof(client));
			//���տͻ��˵�����
			flag = recvfrom(p->sock, (char*)msg, sizeof(t_msg), 0, (sockaddr*)&client, &clientlen);
			(*(p->Num))++;
			if(flag < 0){
				++recv_client_error_num;
				int i = WSAGetLastError();
				cout<<"recvfrom client error! "<<i<<endl;
				char buf[5];
				itoa(i, buf, 10);
				string error = "recvfrom client error! ";
				error.append(buf);
				EnterCriticalSection(&cs);
				log_write(error);
				LeaveCriticalSection(&cs);
				continue;
			}
			string str = "recv msg from client��";
			char buf[5];
			itoa(msg->usr_id, buf, 10);
			str.append(buf).append("��!");
			EnterCriticalSection(&cs);
			log_write(str);
			LeaveCriticalSection(&cs);
			//���Ŀ�ĵص�ID�뱾�ز�ͬ��ɾ��
			if(msg->dst_id != 0){
				++recv_client_error_num;
				string str = "recv error msg!";
				EnterCriticalSection(&cs);
				log_write(str);
				LeaveCriticalSection(&cs);
				continue;
			}

			msg->src_id = 0;
			msg->dst_id = 1;
			EnterCriticalSection(&cs);
			//����Ϣ���뵽��Ϣ������
			map<int, sockaddr_in>::iterator x = p->addr_map->find(msg->usr_id);
			p->msg->push(*msg);
			//client��ַ���뵽clientĿ¼��
			if(x == p->addr_map->end()){
				p->addr_map->insert(pair<int, sockaddr_in>(msg->usr_id, client));
			}
			cout<<">>---->> recv a request from Client ��"<<msg->usr_id<<"��"<<endl;
			if( m_switch == 1 || m_switch == 3){
				cout<<"-->message content: "<<msg->data<<endl;
			}
			cout<<endl;
			LeaveCriticalSection(&cs);
		}
	}
	delete msg;
}

//����Ϣ�������������request��Ϣ
void Thread3(M_params *p)
{
	int flag;
	sockaddr_in server;
	t_msg *msg = new t_msg;
	memset(msg, NULL, sizeof(t_msg));

	while(1){
	//����request
		if(!p->msg->empty()&&!server_q.empty()){
			//��Ϣ���зǿգ��ҷ��������зǿ�
			memcpy(msg ,&(p->msg->front()), sizeof(t_msg));
			//���Ȳ鿴�������б��и÷������Ƿ񻹴���
			map<int, sockaddr_in>::iterator x = p->addr_map->find(server_q.front());
			if(x == p->addr_map->end()){
				//��������ھͰѸ÷������ӷ�����������ɾ��
				if(!server_q.empty())
					EnterCriticalSection(&cs);
					server_q.pop();
					LeaveCriticalSection(&cs);
				continue;
			}
			server = x->second;
			msg->dst_id = server_q.front();
			//�������������Ϣ
			flag = sendto(p->sock, (char*)msg, sizeof(t_msg), 0, (sockaddr*)&server, sizeof(server));
			//���������������+1
			++to_server_num;
			if(flag < 0){
				//���ͳ���+1
				++to_server_error_num;
				int i = WSAGetLastError();
				cout<<"sendto server error! "<<i<<endl;
				char buf[5];
				itoa(i, buf, 10);
				string error = "sendto server error! ";
				error.append(buf);
				EnterCriticalSection(&cs);
				log_write(error);
				LeaveCriticalSection(&cs);
				continue;
			}
			else{
				EnterCriticalSection(&cs);
				char buf[5];
				itoa(server_q.front(), buf, 10);
				string str = "send msg to   server��";
				str.append(buf).append("��!");
				log_write(str);
				cout<<"         Message sendto Server ��"<<server_q.front()<<"�� >>---->>"<<endl;
				if( m_switch == 1 || m_switch == 3){
					cout<<"-->message content: "<<msg->data<<endl;
				}
				cout<<endl;
				//��ѯ������
				server_q.push(server_q.front());
				if(!server_q.empty())
					server_q.pop();

				p->msg->pop();
				LeaveCriticalSection(&cs);
			}
		}
	}
}

void Load_server::start_communication()
{	
	//����������ڴ��ݸ������߳�
    M_params *p1 = (M_params*)malloc(sizeof(M_params));
	M_params *p2 = (M_params*)malloc(sizeof(M_params));
	M_params *p3 = (M_params*)malloc(sizeof(M_params));

	//���͸����շ������߳���Ϣ
	p1->sock = m_socket2;
	p1->Num = &recv_server_num;
	p1->addr_map = &server_map;
	p1->msg = &to_client;

	//���͸����տͻ����߳���Ϣ
	p2->sock = m_socket1;
	p2->Num = &recv_client_num;
	p2->addr_map = &client_map;
	p2->msg = &to_server;

	//���͸����ͷ������߳���Ϣ
	p3->addr_map = &server_map;
	p3->sock = m_socket2;
	p3->msg = &to_server;

	InitializeCriticalSection(&cs);

	//�����߳�
	HANDLE hThread1;
	DWORD ThreadID1;
	hThread1 = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Thread1, p1, 0, &ThreadID1);

	HANDLE hThread2;
	DWORD ThreadID2;
	hThread2 = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Thread2, p2, 0, &ThreadID2);

	HANDLE hThread3;
	DWORD ThreadID3;
	hThread3 = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Thread3, p3, 0, &ThreadID3);

	int flag;
	t_msg *msg = new t_msg;
	int clientlen = sizeof(client);
	int serverlen = sizeof(server);
	
	while(1){
		//ע���źţ�����ʵʱ���ؿ���
		signal(SIGINT,OnCtrlC);

		memset(msg, NULL, sizeof(t_msg));
		memset(&client, NULL, sizeof(client));

		EnterCriticalSection(&cs);
		if(!to_client.empty()&&!client_map.empty()){
			//������͸��ͻ��˵���Ϣ���в�Ϊ�գ��ҿͻ����б�Ϊ��
			memcpy(msg, &(to_client.front()), sizeof(t_msg));
			map<int, sockaddr_in>::iterator f;
			//EnterCriticalSection(&cs);
			//���û�п��õĿͻ��˵�ַ��ɾ������Ϣ
			f = client_map.find(msg->usr_id);
			if(f == client_map.end()){
				++to_client_error_num;
				to_client.pop();
				continue;
			}
			//LeaveCriticalSection(&cs);
			client = f->second;
			++to_client_num;
			//��ͻ��˷�����Ϣ
			flag = sendto(m_socket1, (char*)msg, sizeof(t_msg), 0, (sockaddr*)&client, clientlen);
			if(flag < 0){
				//EnterCriticalSection(&cs);
				++to_client_error_num;
				int i = WSAGetLastError();
				cout<<"sendto client error! "<<i<<endl;
				char buf[5];
				itoa(i, buf, 10);
				string error = "sendto client error! ";
				error.append(buf);
				log_write(error);
				continue;
			}
			else{
				char buf[5];
				itoa(msg->usr_id, buf, 10);
				string str = "send msg to   client��";
				str.append(buf).append("��!");
				log_write(str);
				cout<<"         Message sendto Client ��"<<msg->usr_id<<"�� >>---->>"<<endl;
				if( m_switch == 1 || m_switch == 3){
					cout<<"-->message content: "<<msg->data<<endl;
				}
				cout<<endl;
				to_client.pop();
			}
			//LeaveCriticalSection(&cs);
			if(m_switch == 2 || m_switch == 3){
				cout<<"---------statistic-info--------------"<<endl;
				cout<<"Recv from Client: "<<recv_client_num<<" error: "<<recv_client_error_num<<endl;
				cout<<"Send to   Server: "<<to_server_num<<" error: "<<to_server_error_num<<endl;
				cout<<"Recv from Server: "<<recv_server_num<<" error: "<<recv_server_error_num<<endl;
				cout<<"Send to   Client: "<<to_client_num<<" error: "<<to_client_error_num<<endl;
				cout<<"----------------------------------"<<endl<<endl;
			}
		}
		LeaveCriticalSection(&cs);
	}
	delete msg;
}
