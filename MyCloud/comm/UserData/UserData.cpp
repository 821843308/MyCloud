#include "UserData.h"
#include <json/json.h>
#include <sstream>
#include <string>
#include <fstream>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>


UserData::UserData()
{}
UserData::~UserData()
{}
void UserData::serialize(std::string &output)
{
    Json::Value value;
    value["id"] =id;
    value["name"] = name;
    value["passwd"] =passwd;
    value["cmd"] = cmd;
    value["cmd_state"] = cmd_state;
    value["msg"] = msg;

    Json::FastWriter writer;
    output = writer.write(value);
    return;
}

void UserData::unserialize(std::string &input)
{
    Json::Value value;
    Json::Reader reader;
    reader.parse(input,value);
    id = value["id"].asString();
    name = value["name"].asString();
    passwd = value["passwd"].asString();
    cmd = value["cmd"].asString();
    cmd_state = value["cmd_state"].asString();
    msg = value["msg"].asString();
}


