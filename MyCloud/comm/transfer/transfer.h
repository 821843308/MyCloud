#ifndef _TRANSFER_H_
#define _TRANSFER_H_
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


class Transfer{
public:
static int get_data(int fd,std::string* output)
{
    //1.读取首行数据
    std::string first_line;
    int len;
    int ret=read_line(fd,&first_line);
    if(ret<0){
        perror("error:read_line");
        return -1;
    }
    //2.解析首行数据，获取还需要这次封装的数据大小
    ret=parse_firstline(first_line,&len);
    if(ret<0){
        perror("erro:parse_firstline");
        return -1;
    }
    //3.读取剩下的所有数据
    char buffer[1025];
    ssize_t read_sz;
    while(true){
        memset(buffer,0x00,sizeof(buffer));
        //如果要读取的长度大于缓冲区长度，读取缓冲区大小长度的数据
        //如果小于，则读取len长度的数据
        if(len >= sizeof(buffer)-1){
            read_sz=read(fd,buffer,sizeof(buffer)-1);
        }else{
            read_sz=read(fd,buffer,len);
        }
        if(read_sz<0){
            perror("error:read!");
            return -1;
        }
        //每读取n个字节数据，len数值减小
        buffer[read_sz]='\0';
        len-=read_sz;
        
        (*output)+=buffer;
        //结束标记
        if(len==0){
            //文件读完了
            return 0;
        }else if(len<0){
            perror("error:get_data len<0");
            return -1;
        }
    }   
    return 0;
}

static int set_data(int fd,const std::string& input)
{
    std::stringstream ss;
    std::string output;
    Json::Value value_len;//封装长度
    //将string的长度+"\n"写入output首部,并将input数据尾插写入output中去
    ss<<input.size();
    value_len["len"]=ss.str();
    valueTostr(value_len,&output);
    output+=input;
    //通过write函数将数据写入fd中
    size_t str_sz=output.size();
    size_t len=str_sz;
    while(true){
        ssize_t write_sz=write(fd,(void*)(output.c_str()+str_sz-len),len);
        if(write<0){
            perror("error:set_data write");
            return -1;
        }
        len-=write_sz;
        if(len == 0){
            return 0;
        }
    }
    return 0;
}

static int read_line(int fd,std::string* line)
{
    line->clear();
    while (true) {
      char c = '\0';
      ssize_t read_size = recv(fd, &c, 1, 0);
      if (read_size <= 0) {
        return -1;
      }
      // 如果当前字符是 \r, 把这个情况处理成 \n
      if (c == '\r') {
        // 虽然从缓冲区中读了一个字符, 但是缓冲区并没有把它删掉
        recv(fd, &c, 1, MSG_PEEK);
        if (c == '\n') {
          // 发现 \r 后面一个字符刚好就是 \n, 为了不影响下次循环
          // 就需要把这样的字符从缓冲区中干掉
          recv(fd, &c, 1, 0);
        } else {
          c = '\n';
        }
      }
      // 这个条件涵盖了 \r 和 \r\n 的情况
      if (c == '\n') {
        break;
      }
      line->push_back(c);
    }
    return 0;
}

static int parse_firstline(const std::string& first_line,int* len)
{
    Json::Value value;
    Json::Reader reader;
    if(!reader.parse(first_line,value)){
        perror("error:parse_firstline wrong!");
        return -1;
    }
    *len=atoi(value["len"].asString().c_str());
    return 0;
}

static int valueTostr(const Json::Value& input,std::string* output)
{
    Json::FastWriter writer;
    (*output)=writer.write(input);
    return 0;
}

static int strTovalue(const std::string& input,Json::Value* output)
{
    Json::Reader reader;
    if(!reader.parse(input,*output)){
        perror("error:parse_firstline wrong!");
        return -1;
    }
    return 0;
}
};//end class transfer
#endif
