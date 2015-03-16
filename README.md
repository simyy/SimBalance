##simpleBalance
>UDP实现的负载均衡程序，包含服务器和服务器，通过C++实现。

>A demo of load balancing in windows.
>It contains client, server and loadbance, writen in c++ with VS.


####Start
开始方法：打开VS并编辑配置文件xml

*  open a vs, such as vs2008.
*  open this project in it
*  build and start
*  edit config in xml

####Use
使用C++、STL、多线程、socket、UDP、XML、log等

*  c++
*  STL
*  multi threading
*  socket
*  udp
*  xml config
*  run log


####More

  In it, there are three parts, such as load balance, client, server. As a win32 software, it just start by command.

  Considering the effections in the communication of the local area network, it use UDP to communicate with each other. For failure in communications, it use restransmisson method to avoid lose of message.

  What's more, it has the functions of log and config files.
