#ifndef _CLIENT_H_
#define _CLIENT_H_

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
#include "../comm/UserData/UserData.h"
#include "../Window/Window.h"
#include "../md5/md5.h"

typedef struct sockaddr sockaddr;
typedef struct sockaddr_in sockaddr_in;


class Client{
public:
    //客户端初始化
    int init(const std::string& server_ip,short server_port);
    //设置用户信息
    void choose_cmd(Window* pwindow);
    //登录
    void login(Window* pwindow);
    //注册
    void regist(Window* pwindow);
    //下载
    int download(const char* filename);
    
    //上传
    int upload(const char* path,const char* filename);

    //删除服务器文件
    int deletefile(const char*filename);

public:
    int sock_;
    sockaddr_in server_addr_;
    UserData user_data_;
    MD5 md5;
};//end class client 

#endif
                   
                   
                   
                
