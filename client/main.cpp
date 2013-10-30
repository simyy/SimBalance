#include "Util.h"
#include "client.h"

void main()
{
	int n;
	int port;
	unsigned id;
	unsigned user_id;
	char addr[20];
	cout<<"************************************"<<endl;
	cout<<"select mode(Load-balancing Server):"<<endl;
	cout<<"1: test mode.....only 127.0.0.1"<<endl;
	cout<<"2: input IP which you want"<<endl;
	cout<<"************************************"<<endl;
	do{
		cin>>n;
		if(n!=1&&n!=2)
			cout<<"input error! Input again!"<<endl;
	}while(n!=1&&n!=2);
	if(n == 1){
		cout<<"************************************"<<endl;
		cout<<"This is a client"<<endl;
		cout<<"Please input your user_id/id/port: "<<endl<<" ";
		cin>>user_id>>id>>port;
		cout<<"************************************"<<endl<<endl;
		Client m_client(port, id, user_id);
		m_client.init_client();
	}else{
		cout<<"************************************"<<endl;
		cout<<"This is a client"<<endl;
		cout<<"Please input your user_id/id/port: "<<endl<<" ";
		cin>>user_id>>id>>port;
		cout<<"Input load balancing Server IP:"<<endl;
		cin>>addr;
		cout<<"************************************"<<endl<<endl;
		Client m_client(port, id, user_id, addr);
		m_client.init_client();
	}
}