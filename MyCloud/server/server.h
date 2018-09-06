#ifndef _SERVER_H_
#define _SERVER_H_

#include <sys/epoll.h>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sstream>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <string>
#include "../myDB/myDB.h"
#include "../comm/transfer/transfer.h"
#include "../comm/UserData/UserData.h"
#include <fcntl.h>

#define EPOLL_SIZE 10000

typedef struct sockaddr sockaddr;
typedef struct sockaddr_in sockaddr_in;

class Server;

typedef struct Data{
    int connect_fd;
    int epoll_fd;
    Server* server;
    MyDB mydb;
    UserData user_data;
}Data; 

//服务器
class Server{
public:
    int start(const std::string& ip,short port);
    int epoll_work();
    
private:
    static void* thread_entry(void* arg);
    //处理连接请求
    int  process_request(Data* data);
    int get_filelist(Data* data);
    //登录功能
    int sign_in(Data* data);
    //注册功能
    int sign_up(Data* data);
    //解析取出的数据
    int parse_data(Data* data,const std::string& input);
    //下载
    int download(Data* data);
    //上传
    int upload(Data* data);
    //删除
    int deletefile(Data* data);
private:
    //服务器端绑定的sock_
    int listen_sock;
};//end class server

#endif
