#include "Util.h"

class Load_server{
public:
	Load_server();
	Load_server(const int port);
	~Load_server();

	//��ʼ��
	void init_server();
	//��ʼͨ��
	void start_communication();

private:
	//�������ӿͻ���/��������SOCKET
	SOCKET m_socket1;
	SOCKET m_socket2;
	
	//��ͬ�˿ڱ��ص�ַ��Ϣ
	sockaddr_in local1;
	sockaddr_in local2;
	//�ͻ��ˡ���������Ϣ
	sockaddr_in client;
	sockaddr_in server;

	//<id����ַ�˿�>�����洢���������ͻ���ID/��ַ��Ϣ
	map<int, sockaddr_in> server_map;
	map<int, sockaddr_in> client_map;
	//��Ϣ���У���������Ϳͻ��˷��ͣ�
	queue<t_msg> to_server;
	queue<t_msg> to_client;
	//������Ϣ����
	int recv_client_num;
	int recv_server_num;
};