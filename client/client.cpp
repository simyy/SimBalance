#include "client.h"
#include "Util.h"

//����������Ϣ�ķ��ͺ͹ر�
int i=0;

Client::Client()
{
	id = 1;
	user_id = 1;

	send_num = 0;
	recv_num = 0;
	send_error_num = 0;
	recv_error_num = 0;

	local.sin_family = AF_INET;
	local.sin_port = htons(M_PORT-1);
	local.sin_addr.s_addr = INADDR_ANY;

	load_server.sin_family = AF_INET;
	load_server.sin_port = htons(M_PORT);
	load_server.sin_addr.s_addr = inet_addr("127.0.0.1");
}

Client::Client(const int port, const unsigned m_id, const unsigned m_user_id)
{
	local.sin_family = AF_INET;
	local.sin_port = htons(port-1);
	local.sin_addr.s_addr = INADDR_ANY;

	load_server.sin_family = AF_INET;
	load_server.sin_port = htons(M_PORT);
	load_server.sin_addr.s_addr = inet_addr("127.0.0.1");

	id = m_id;
	user_id = m_user_id;

	send_num = 0;
	recv_num = 0;
	send_error_num = 0;
	recv_error_num = 0;
}

Client::Client(const int port, const unsigned m_id, const unsigned m_user_id, const char* addr)
{
	local.sin_family = AF_INET;
	local.sin_port = htons(port);
	local.sin_addr.s_addr = INADDR_ANY;

	load_server.sin_family = AF_INET;
	load_server.sin_port = htons(M_PORT);
	load_server.sin_addr.s_addr = inet_addr(addr);

	id = m_id;
	user_id = m_user_id;

	send_num = 0;
	recv_num = 0;
	send_error_num = 0;
	recv_error_num = 0;
}

Client::~Client()
{
	closesocket(m_socket);
	WSACleanup();
}

void Client::init_client()
{
	//��ʼ��
	WSADATA wsaData;
	WORD sockVersion = MAKEWORD(2,2);
	if(WSAStartup(sockVersion, &wsaData) != 0)
	{
		return;
	}
	//����socket
	m_socket = socket(AF_INET, SOCK_DGRAM, 0);
	if(m_socket < 0){
		cout<<"socket error"<<endl;
		return;
	}
	//��socket
	int flag;
	flag = bind(m_socket, (sockaddr*)&local, sizeof(local));
	if(flag < 0){
		cout<<"bind error!"<<endl;
		return;
	}
	//RECV��ʱ����
	DWORD TimeOut=1000*5;   //���ý��ճ�ʱ5��
	if(setsockopt(m_socket, SOL_SOCKET, SO_RCVTIMEO,(char *)&TimeOut,sizeof(TimeOut))==SOCKET_ERROR)
	{
		cout<<"����ʧ��"<<endl;
	}

	start_communication();
}

//SIGINT�źŻص�����
void OnCtrlC(int)
{
	i =0;
	cout<<"------>close Heartbeat mode<-------"<<endl;
}

void Client::start_communication()
{
	int flag;
	t_msg *msg = new t_msg;
	int msg_type = 0;

	//����ͨ��״̬
	while(1){
		signal(SIGINT, OnCtrlC);

		do{
			cout<<"--------->select-msg-type<---------"<<endl;
			cout<<"0 ��Time request"<<endl;
			cout<<"2 : Heartbeat request "<<endl;
			cout<<"---------------------------------"<<endl;
			cin>>msg_type;//����ʾ��������ʽ
			if(msg_type != 0 && msg_type != 2)
				cout<<"input error num, input again!"<<endl;
		}while(msg_type != 0 && msg_type != 2);

		if(msg_type == 2){
			i =1;//iΪ1�����������/���վͱ�Ϊѭ��ģʽ
			cout<<"----->>start Heartbeat recv!<<-----"<<endl;
		}

		do{
			if(1 == i){
				//��������ģʽ��Ƶ����2��
				cout<<"--------->>Heartbeat mode<<--------"<<endl;
				Sleep(2000);//2��
			}
			//�����Ϣ��Ϣ
			memset(msg, NULL, sizeof(t_msg));
			msg->usr_id = user_id;
			msg->src_id = id;
			msg->dst_id = 0;
			msg->msg_type = msg_type;
			strcpy(msg->data ,"Time requset");
		
			cout<<"----------one request and response----------"<<endl;
			cout<<"         send request to server >>---->>"<<endl;
			cout<<"msg: "<<msg->data<<endl<<endl;
			//����ʱ��������Ϣ,���ܷ��ʹ���+1
			++send_num;
			flag = sendto(m_socket, (char*)msg, sizeof(t_msg), 0, (struct sockaddr*)&load_server, sizeof(load_server));
			if(flag < 0){
				cout<<"sendto error!"<<endl;
				cout<<WSAGetLastError()<<endl;
				++send_error_num;
				Sleep(5000);//�����ӳ�5��
				continue;
			}

			memset(msg, NULL, sizeof(t_msg));
			int load_server_len = sizeof(load_server);
			//������ϢӦ��,���ܽ��մ���+1
			recv_num++;
			flag = recvfrom(m_socket, (char*)msg, sizeof(t_msg), 0, (struct sockaddr*)&load_server, &load_server_len);
			if(flag < 0){
				//��ʱ���ط�
				cout<<"	Timeout, retry!!!!"<<endl;
				cout<<WSAGetLastError()<<endl;
				cout<<endl;
				++send_error_num;
				cout<<"Send num: "<<send_num<<"  error num: "<<send_error_num<<endl;
				cout<<"Recv num: "<<recv_num<<"  error num: "<<recv_error_num<<endl;
				cout<<"--------------------------------------------"<<endl<<endl;
				//Sleep(5000);
				continue;
			}
			//ID�Ƿ����
			if(msg->dst_id != user_id){
				++recv_error_num;
				cout<<"Send num: "<<send_num<<"  error num: "<<send_error_num<<endl;
				cout<<"Recv num: "<<recv_num<<"  error num: "<<recv_error_num<<endl;
				cout<<"--------------------------------------------"<<endl<<endl;
				continue;
			}
			cout<<">>---->> recv response from server"<<endl;
			cout<<"msg: "<<msg->data<<endl<<endl;
			cout<<"Send num: "<<send_num<<"  error num: "<<send_error_num<<endl;
			cout<<"Recv num: "<<recv_num<<"  error num: "<<recv_error_num<<endl;
			cout<<"--------------------------------------------"<<endl<<endl;
			cout<<endl;
		}while(i);
		system("pause");
	}
	delete msg;
}