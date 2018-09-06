#ifndef _USERDATA_H_
#define _USERDATA_H_
#include <iostream>
#include <json/json.h>

class UserData
{
public:
    UserData();
    void serialize(std::string& output);
    void unserialize(std::string& input);
    ~UserData();
public:
    std::string id;
    std::string name;
    std::string passwd;
    std::string cmd;
    std::string cmd_state;//保存客户端请求处理的情况,S表示成功，F表示失败
    std::string msg;//保存客户端请求的信息 

};
#endif
