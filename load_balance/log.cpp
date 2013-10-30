#include "log.h"

void getTime(string& m_time)
{
	time_t t = time(0);
	char now[20]; 
	strftime(now, sizeof(now), "%Y-%m-%d %H:%M:%S", localtime(&t)); 
	m_time = now;
}

bool log_open()
{
	fstream _file;
	_file.open(FILENAME,ios::in);
	if(!_file){
		cout<<FILENAME<<"没有被创建,创建一个LOG"<<endl<<endl;
		_file.open(FILENAME, ios::_Nocreate);
		return true;
	}
	//cout<<FILENAME<<"已经存在";
	return true;
}

bool log_write(string& error)
{
	log_open();

	string time;
	getTime(time);
	error = time + error + ";\n";
	fstream _file;
	_file.open(FILENAME, ios::app);
	_file.write(error.c_str(), strlen(error.c_str()));

	return true;
}

//void main()
//{
//	file_open();
//	string error = "adfadf";
//	error += "\n";
//	file_write(error);
//	file_write(error);
//}