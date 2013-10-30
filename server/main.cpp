#include "Util.h"
#include "server.h"

void main()
{
	int n;
	int port, id;
	char addr[20];
	cout<<"*********************************"<<endl;
	cout<<"select mode(Load-balancing Server):"<<endl;
	cout<<"1: test mode ...only 127.0.0.1"<<endl;
	cout<<"2: input a IP you want"<<endl;
	cout<<"*********************************"<<endl;
	do{
		cin>>n;
		if(n!=1&&n!=2)
			cout<<"input error! Input again!"<<endl;
	}while(n!=1&&n!=2);
	if(n == 1){
		cout<<"*********************************"<<endl;
		cout<<"This is a server"<<endl;
		cout<<"Please input id and port: ";
		cin>>id>>port;
		cout<<"*********************************"<<endl<<endl;
		Server server(port, id);
		server.init_server();
	}else{
		cout<<"*********************************"<<endl;
		cout<<"This is a server"<<endl;
		cout<<"Please input id and port: ";
		cin>>id>>port;
		cout<<"Input load balancing Server IP:"<<endl;
		cin>>addr;
		cout<<"*********************************"<<endl<<endl;
		Server server(port, id, addr);
		server.init_server();
	}
}