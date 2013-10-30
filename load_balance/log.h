#include "Util.h"

#define FILENAME "log.txt"

//向Log.txt写入信息，如果不存在，则先创建一个log.txt文件
bool log_write(string& error);