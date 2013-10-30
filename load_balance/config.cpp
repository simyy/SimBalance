#include "Util.h"

CString GetAppPath()
{//获取应用程序根目录
	TCHAR modulePath[MAX_PATH];
	GetModuleFileName(NULL, modulePath, MAX_PATH);
	CString strModulePath(modulePath);
	strModulePath = strModulePath.Left(strModulePath.ReverseFind(_T('\\')));
	return strModulePath;
}

string itos(int a)
{
	stringstream sstrm;
	sstrm << a;
	return sstrm.str();
}

bool m_file(string name)
{
	fstream _file;
	_file.open(name.c_str(), ios::in);
	if(!_file)
	{
		TiXmlDocument *myDocument = new TiXmlDocument();
		TiXmlElement *RootElement = new TiXmlElement("Load_balancing_server");
		myDocument->LinkEndChild(RootElement);
		RootElement->SetAttribute("ID", "0");
		myDocument->SaveFile(name.c_str());//保存到文件
	}
	return true;
}

bool AddXmlFile(string& szFileName, int& m_ID, sockaddr_in& m_sockaddr)
{
	string m_ip = inet_ntoa(m_sockaddr.sin_addr);
	string m_id = itos(m_ID);
	string m_port = itos(ntohs(m_sockaddr.sin_port));

	CString appPath = GetAppPath();
	string seperator = "\\";
	string fullPath = appPath.GetBuffer(0) + seperator+szFileName;

	m_file(fullPath);
	TiXmlDocument	*myDocument = new TiXmlDocument(fullPath.c_str());
	myDocument->LoadFile();
	TiXmlElement *RootElement = myDocument->RootElement();


	//创建一个元素并连接。
	TiXmlElement *server = new TiXmlElement("Server");
	//设置元素的属性。
	server->SetAttribute("ID", m_id.c_str());
	//创建ip子元素、port子元素并连接。
	RootElement->LinkEndChild(server);

	//创建ip子元素、port子元素并连接。
	TiXmlElement *ip = new TiXmlElement("ip");
	TiXmlElement *port = new TiXmlElement("port");
	server->LinkEndChild(ip);
	server->LinkEndChild(port);
	//设置ip子元素和port子元素的内容并连接。
	TiXmlText *ipContent = new TiXmlText(m_ip.c_str());
	TiXmlText *portContent = new TiXmlText(m_port.c_str());
	ip->LinkEndChild(ipContent);
	port->LinkEndChild(portContent);

	//RootElement->InsertEndChild(*server);
	
	myDocument->SaveFile(fullPath.c_str());//保存到文件

	return true;
}

bool DeleteXmlFile(string& szFileName, int& m_ID, sockaddr_in& m_sockaddr)
{
	string m_ip = inet_ntoa(m_sockaddr.sin_addr);
	string m_id = itos(m_ID);
	string m_port = itos(ntohs(m_sockaddr.sin_port));

	CString appPath = GetAppPath();
	string seperator = "\\";
	string fullPath = appPath.GetBuffer(0) +seperator+szFileName;

	TiXmlDocument	*myDocument = new TiXmlDocument(fullPath.c_str());
	myDocument->LoadFile();
	TiXmlElement *RootElement = myDocument->RootElement();

	TiXmlElement* pNode  = NULL;
	for(pNode = RootElement->FirstChildElement(); pNode; pNode = pNode->NextSiblingElement()){
		if(!strcmp(pNode->Value(), "Server") && !strcmp(pNode->Attribute("ID"), m_id.c_str())){
			cout<<pNode->Value()<<pNode->Attribute("ID")<<endl;
			RootElement->RemoveChild(pNode);
			break;
		}
	}
	myDocument->SaveFile(fullPath.c_str());//保存到文件
	return true;
}

//int main()
//{
//	string fileName = "info.xml";
//	int id=13;
//	sockaddr_in server;
//
//	server.sin_addr.S_un.S_addr = inet_addr("127.56.56.1");
//	server.sin_port=htons(6789);
//	//AddXmlFile(fileName,id,server);
//
//	DeleteXmlFile(fileName,id,server);
//
//	return 0;
//
//}