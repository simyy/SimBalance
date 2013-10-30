#include "Util.h"

class Server{
public:
	Server();
	Server(const int port, const int m_id);
	Server(const int port, const int m_id, const char* load_serv_addr);
	~Server();

	//服务器初始化（创建套接字、绑定本地地址端口）
	void init_server();
	//与负载均衡服务器进行通信
	void start_communication();

private:
	//本地、负载均衡服务器信息
	sockaddr_in server;	     
	sockaddr_in load_server; 

	//本地套接字标识符
	SOCKET m_socket;
	//服务器标识ID，唯一
	int id;            

	//接收消息队列
	queue<t_msg> recv_client_msg;

	//发送、接收总数
	int send_num;
	int recv_num;

	//发送、接收错误数
	int send_error_num;
	int recv_error_num;
};