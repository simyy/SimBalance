#include "Util.h"

class Client{
public:
	Client();
	Client(const int port, const unsigned id, const unsigned m_user_id);
	Client(const int port, const unsigned id, const unsigned m_user_id, const char* addr);
	~Client();

	//��ʼ��
	void init_client();
	//��ʼͨ��
	void start_communication();

private:
	//����socket
	SOCKET m_socket;

	//���ص�ַ��Ϣ
	sockaddr_in local;
	//���ؾ���˵�ַ��Ϣ
	sockaddr_in load_server;
	
	//���ظ�
	unsigned    id;
	//�û�ID��user_id 
	unsigned    user_id;

	//���ͺͽ�������
	int			send_num;
	int			recv_num;
	//���ͺͽ��մ�������
	int			send_error_num;
	int			recv_error_num;
};