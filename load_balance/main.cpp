#include "Util.h"
#include "Load_balance.h"

#pragma comment(lib, "ws2_32.lib") 

void main()
{
	Load_server loadSever;
	loadSever.init_server();
}