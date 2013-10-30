#include "Util.h"

class Server{
public:
	Server();
	Server(const int port, const int m_id);
	Server(const int port, const int m_id, const char* load_serv_addr);
	~Server();

	//��������ʼ���������׽��֡��󶨱��ص�ַ�˿ڣ�
	void init_server();
	//�븺�ؾ������������ͨ��
	void start_communication();

private:
	//���ء����ؾ����������Ϣ
	sockaddr_in server;	     
	sockaddr_in load_server; 

	//�����׽��ֱ�ʶ��
	SOCKET m_socket;
	//��������ʶID��Ψһ
	int id;            

	//������Ϣ����
	queue<t_msg> recv_client_msg;

	//���͡���������
	int send_num;
	int recv_num;

	//���͡����մ�����
	int send_error_num;
	int recv_error_num;
};