#include "Util.h"

class Load_server{
public:
	Load_server();
	Load_server(const int port);
	~Load_server();

	//初始化
	void init_server();
	//开始通信
	void start_communication();

private:
	//用于连接客户端/服务器的SOCKET
	SOCKET m_socket1;
	SOCKET m_socket2;
	
	//不同端口本地地址信息
	sockaddr_in local1;
	sockaddr_in local2;
	//客户端、服务器信息
	sockaddr_in client;
	sockaddr_in server;

	//<id，地址端口>用来存储服务器、客户端ID/地址信息
	map<int, sockaddr_in> server_map;
	map<int, sockaddr_in> client_map;
	//消息队列（向服务器和客户端发送）
	queue<t_msg> to_server;
	queue<t_msg> to_client;
	//接收消息总数
	int recv_client_num;
	int recv_server_num;
};