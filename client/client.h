#include "Util.h"

class Client{
public:
	Client();
	Client(const int port, const unsigned id, const unsigned m_user_id);
	Client(const int port, const unsigned id, const unsigned m_user_id, const char* addr);
	~Client();

	//初始化
	void init_client();
	//开始通信
	void start_communication();

private:
	//本地socket
	SOCKET m_socket;

	//本地地址信息
	sockaddr_in local;
	//负载均衡端地址信息
	sockaddr_in load_server;
	
	//可重复
	unsigned    id;
	//用户ID：user_id 
	unsigned    user_id;

	//发送和接收总数
	int			send_num;
	int			recv_num;
	//发送和接收错误总数
	int			send_error_num;
	int			recv_error_num;
};