#include "server.h"
#include <unistd.h>
#include <sys/sendfile.h>

int Server::start(const std::string& ip,short port)
{
    //1.建立tcp连接
    listen_sock=socket(AF_INET,SOCK_STREAM,0);
    if(listen_sock<0){
        perror("socket");
        return -1;
    }
    std::cout<<"socket ok"<<std::endl;

    int ret,opt=1;
    setsockopt(listen_sock,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
    sockaddr_in addr;
    addr.sin_family=AF_INET;
    addr.sin_addr.s_addr=inet_addr(ip.c_str());
    addr.sin_port=htons(port);
    //绑定
    ret=bind(listen_sock,(sockaddr*)&addr,sizeof(addr));
    if(ret<0){
        perror("bind");
        return -1;
    }
    std::cout<<"bind ok"<<std::endl;
    //监听
    ret=listen(listen_sock,5);
    if(ret<0){
        perror("listen");
        return -1;
    }
    std::cout<<"listen ok"<<std::endl;

    return 0;
}

int Server::epoll_work()
{
    //设置epoll_fd
    int epoll_fd=epoll_create(EPOLL_SIZE);
    if(epoll_fd<0){
        perror("error:epoll_create");
        return -1;
    }
    std::cout<<"epoll_create ok"<<std::endl;

    struct epoll_event event;
    event.events=EPOLLIN;
    event.data.fd=listen_sock;
    int ret=epoll_ctl(epoll_fd,EPOLL_CTL_ADD,listen_sock,&event);
    if(ret<0){
        std::cout<<"epoll_ctl listen_sock faile"<<std::endl;
    }
    //循环处理客户端连接
    while(true){
        struct epoll_event events[EPOLL_SIZE];
        std::cout<<"befor epoll_wait "<<std::endl;
        int sz=epoll_wait(epoll_fd,events,EPOLL_SIZE,-1);
        std::cout<<"after epoll_wait"<<std::endl;
        if(sz == -1)
        {
            std::cout<<"epoll_wait error"<<std::endl;
            continue;
        }
        else
        {
            std::cout<<sz<<std::endl;
            /* continue; */
        }
        
        for(int i = 0;i < sz;i++)
        {
            if(events[i].data.fd == listen_sock)
            {
                sockaddr_in client_addr;
                socklen_t len=sizeof(client_addr);
                int connect_fd=accept(listen_sock,(sockaddr*)&client_addr,&len);
                if(connect_fd<0){
                    std::cout<<"accept falie"<<std::endl;
                    continue;
                }
                std::cout<<"client:"<<inet_ntoa(client_addr.sin_addr)<<" "<<ntohs(client_addr.sin_port)<<std::endl;
                struct epoll_event ev;
                ev.data.fd = connect_fd;
                ev.events = EPOLLIN;
                int ret = epoll_ctl(epoll_fd,EPOLL_CTL_ADD,connect_fd,&ev);
                if(ret < 0)
                {
                    std::cout<<"epoll_ctl error"<<std::endl;
                    return -1;
                }

            }
            else
            {
                std::cout<<"处理请求"<<std::endl;
                Data* data=new Data;
                data->connect_fd=events[i].data.fd;
                data->epoll_fd=epoll_fd;
                data->server=this;
                int ret=data->mydb.init();
                if(ret<0){
                     perror("error:sign_in mydb.init()");
                }
                
                
                ret= data->server->process_request(data);
                if(ret<0){
                    epoll_ctl(data->epoll_fd,EPOLL_CTL_DEL,data->connect_fd,NULL);
                    close(data->connect_fd);
                    std::cout<<"connet_fd close,epoll_ctldel"<<std::endl;
                }
                delete data;

            }
        }
    }
    close(epoll_fd);
}

int Server::process_request(Data* data)
{
    int ret;
    //1.取出数据
    std::string result;
    ret=Transfer::get_data(data->connect_fd,&result);
    if(ret<0){
        std::cout<<"error:process_request get_data"<<std::endl;
        return -1;
    }
    std::cout<<"result:"<<result<<std::endl;
    //2.进行反序列化
    data->user_data.unserialize(result);
    std::string cmd=data->user_data.cmd;
    //3.根据客户端命令进行响应操作
    if(cmd == "sign_in")
    {
        std::string output;
        int ret = sign_in(data);
        if(ret == 0)
        {
            get_filelist(data);
        }
        data->user_data.serialize(output);
        Transfer::set_data(data->connect_fd,output);
    }
    else if(cmd == "sign_up")
    {
        std::string output;
        int ret = sign_up(data);
        if(ret == 0)
        {
            get_filelist(data);
        }
        data->user_data.serialize(output);
        Transfer::set_data(data->connect_fd,output);
    }
    else if(cmd == "upload")
    {
        int ret = upload(data);
        if(ret == 0)
        {
            std::cout<<"upload success"<<std::endl;
        }
    }
    else if(cmd == "download")
    {
        int ret = download(data);
        if(ret == 0)
        {
            std::cout<<"download success"<<std::endl;
        }
    }
    else if(cmd == "deletefile")
    {
        std::string output;
        int ret = deletefile(data);
        if(ret == 0)
        {
            data->user_data.cmd_state="S";
            data->user_data.msg = "删除成功";
        }
        else
        {
            data->user_data.cmd_state="F";
            data->user_data.msg = "删除失败";
        }
        data->user_data.serialize(output);
        Transfer::set_data(data->connect_fd,output);
    }
    return 0;
}

//登录功能
int Server::sign_in(Data* data)
{
    //1.进行数据库查询,如果有当前用户返回失败
    std::stringstream ss;
    std::vector<std::string> res;
    ss<<"select password from user where id like '"<<
        data->user_data.id.c_str()<<"';";
    data->mydb.exec_sql(ss.str());
    res=data->mydb.get_res();
    //2.根据查询的情况与用户提交的信息对比得出结果
    if(res.empty()){
        data->user_data.cmd_state="F";
        data->user_data.msg="没有此id的用户！";
        return -1;
    }
    if(res[0] != data->user_data.passwd){
        data->user_data.cmd_state="F";
        data->user_data.msg="密码不正确！";
        return -1;
    }
    data->user_data.cmd_state="S";
    return 0;
}
//注册功能
int Server::sign_up(Data* data)
{
    //1.进行数据库查询,如果有当前用户返回失败
    std::stringstream ss;
    std::vector<std::string> res;
    ss<<"select id from user where id like '"
        <<data->user_data.id.c_str()<<"';";
    data->mydb.exec_sql(ss.str());
    res=data->mydb.get_res();
    //2.根据查询的情况与用户提交的信息对比得出结果
    if(!res.empty()){
        //已经有注册过这个id的用户，返回失败
        data->user_data.cmd_state="F";
        data->user_data.msg="此id已经被注册！";
        return -1;
    }
    //组装sql语句
    ss.str("");
    ss<<"insert into user(id,name,password) values('"<<
        data->user_data.id<<"','"<<data->user_data.name<<"','"
        <<data->user_data.passwd<<"');";
    data->mydb.exec_sql(ss.str());
    data->user_data.cmd_state="S";

    return 0;
}
int Server::get_filelist(Data* data)
{
    std::stringstream ss;
    std::vector<std::string> res;
    ss<<"select file_name from user_file where id like '"
                <<data->user_data.id<<"';";
    data->mydb.exec_sql(ss.str());
    res=data->mydb.get_res();
    std::cout<<"get_filelist:"<<res.size()<<std::endl;
    for(int i=0;i<res.size();++i){
    data->user_data.msg+=res[i];
    data->user_data.msg+=" ";
        
    }
    return 0;
}
//下载
int Server::download(Data* data)
{
    //查找下载文件的filemd5
    std::stringstream ss;
    std::vector<std::string> res;
    ss<<"select file_md5 from user_file where id like '"
        <<data->user_data.id.c_str()<<"' and file_name like '"<<
        data->user_data.msg.c_str()<<"';";
    data->mydb.exec_sql(ss.str());
    res=data->mydb.get_res();
    if(res.empty())
    {
        std::cout<<"user don't have this file,can't download!"<<std::endl;
        return -1;
    }

    std::string filemd5 = res[0];
    char path[100];
    sprintf(path,"../MyCloud_files/files/%s",filemd5.c_str());
    struct stat sta;
    stat(path,&sta);
    int fd = open(path,O_RDONLY);
    
    sendfile(data->connect_fd,fd,NULL,sta.st_size);
    return 0;
}

int ishavefilemd5(Data* data,const std::string filemd5)
{
    std::stringstream ss;
    std::vector<std::string> res;
    ss<<"select count from filemd5_count where file_md5 like '"
                <<filemd5.c_str()<<"';";
    data->mydb.exec_sql(ss.str());
    res=data->mydb.get_res();
    if(res.empty())
    {
        std::cout<<"ishavefilemd5: false"<<std::endl;
        return -1;
    }
    else
    {
        int count = atoi(res[0].c_str());

        ss.str("");
        ss<<"update filemd5_count set count=count+1 where file_md5='"
             <<filemd5.c_str()<<"';";
        int ret = data->mydb.exec_sql(ss.str());
        if(ret < 0)
        {
            std::cout<<"ishavefilemd5: true but +1 error"<<std::endl;
            return -2;
        }
        else 
            std::cout<<"ishavefilemd5: true"<<res[0]<<"+1"<<std::endl;
        return count+1;

    }

}
int add_filemd_count(Data* data,std::string& file_md5)
{
    std::stringstream ss;
    ss<<"insert into filemd5_count(file_md5,count) values('"<<
        file_md5<<"',1);";
    data->mydb.exec_sql(ss.str());
    return 0;

}
int add_user_file(Data* data,std::string &file_name,const std::string &file_md5)
{
    std::stringstream ss;
    ss.str("");
    ss<<"insert into user_file(id,file_name,file_md5) values('"<<
        data->user_data.id<<"','"<<file_name<<"','"<<file_md5<<"');";
    data->mydb.exec_sql(ss.str());
    return 0;
}
//上传
int Server::upload(Data* data)
{
    Json::Value value;
    Transfer::strTovalue(data->user_data.msg,&value);
    std::string filename;
    int filesize;
    std::string filemd5;
    filename = value["filename"].asString();
    filesize = value["filesize"].asInt();
    filemd5 = value["filemd5"].asString();

    int ret = ishavefilemd5(data,filemd5);
    //没有该文件需要客户端上传
    if(ret == -1)
    {
        data->user_data.cmd_state = "Please Send";
        std::string upload_msg;
        Json::Value value;
        value["file_offset"] = 0;

        Transfer::valueTostr(value,&upload_msg);
        data->user_data.msg = upload_msg;

        std::string jsonstr;
        data->user_data.serialize(jsonstr);
        Transfer::set_data(data->connect_fd,jsonstr);
        
        //创建文件名，准备接收
        std::string path;
        path = "../MyCloud_files/files/"+filemd5;
        int fd = open(path.c_str(),O_CREAT|O_RDWR,0666);
        if(fd == -1)
        {
            std::cout<<"open fd error"<<std::endl;
        }

        char buf[1024];
        int rdsz = -1;
        while((rdsz = recv(data->connect_fd,buf,64,0)) >0)
        {
            buf[rdsz] = '\0';

            std::cout<<buf<<std::endl;
            int wrsz = write(fd,buf,rdsz);
            if(wrsz != rdsz)
            {
                std::cout<<"writ false"<<std::endl;
                return -1;
            }
            if(rdsz < 64)
                break;

        }
        close(fd);
        //向数据库添加信息
        add_user_file(data,filename,filemd5);
        add_filemd_count(data,filemd5);

        data->user_data.cmd_state = "S";
        data->user_data.msg = "上传成功！";

        jsonstr ="";
        data->user_data.serialize(jsonstr);
        int ret = Transfer::set_data(data->connect_fd,jsonstr);
        if(ret < 0)
        {
            std::cout<<"set_data error"<<std::endl;
        }
        else
        {
            std::cout<<jsonstr<<std::endl;
        }
/*         int ret = sendfile(fd,data->connect_fd,NULL,filesize); */
/*         if(ret != filesize) */
/*         { */
/*             std::cout<<"sendfile erro"<<std::endl; */
/*             return -1; */
/*         } */
/*         else */
/*         { */
/*             add_user_file(data,filename,filemd5); */
/*             add_filemd_count(data,filemd5); */

/*         } */
/*             close(fd); */
    }
    else if(ret == -2)
    {
    }
    else//成功妙传
    {
        add_user_file(data,filename,filemd5);

        data->user_data.cmd_state = "S";
        data->user_data.msg = "秒传成功！";

        std::string jsonstr;
        data->user_data.serialize(jsonstr);
        Transfer::set_data(data->connect_fd,jsonstr);
    }

    return 0;
}

//删除文件 
int Server::deletefile(Data* data)
{
    std::string filemd5;
    std::stringstream ss;
    std::vector<std::string> res;
    ss<<"select file_md5 from user_file where id like '"
        <<data->user_data.id.c_str()<<"' and file_name like '"<<
        data->user_data.msg.c_str()<<"';";

    std::cout<<"deletefile id:"<<data->user_data.id.c_str()<<std::endl;
    std::cout<<"deletefile file_name:"<<data->user_data.msg.c_str()<<std::endl;
    data->mydb.exec_sql(ss.str());
    res=data->mydb.get_res();
    if(res.empty())
    {

        std::cout<<"no file_md5"<<std::endl;
        return -1;
    }

    filemd5 = res[0];
    std::cout<<"deletefile filemd5:"<<filemd5<<std::endl;

    ss.str(""); 
    ss<<"select count from filemd5_count where file_md5 like '"
        <<filemd5.c_str()<<"';";
    data->mydb.exec_sql(ss.str());
    res=data->mydb.get_res();
    if(res.empty())
    {

        std::cout<<"no count"<<std::endl;
        return -1;
    }
    int count = atoi(res[0].c_str());
    if(count == 1)
    {
        std::string path = "../MyCloud_files/files/"+filemd5;
        int ret = unlink(path.c_str());
        if(ret < 0)
        {
            std::cout<<"rm MyCloud_files/files/filemd5 error"<<std::endl;
            return -1;
        }
        ss.str(""); 
        ss<<"delete from user_file where id like '"
            <<data->user_data.id.c_str()<<"' and file_name like '"<<
            data->user_data.msg.c_str()<<"';";
        data->mydb.exec_sql(ss.str());

        ss.str(""); 
        ss<<"delete from filemd5_count where file_md5 like '"
            <<filemd5<<"';";
        data->mydb.exec_sql(ss.str());
    
    }
    else
    {
        ss.str(""); 
        ss<<"delete from user_file where id like '"
            <<data->user_data.id.c_str()<<"' and file_name like '"<<
            data->user_data.msg.c_str()<<"';";
        data->mydb.exec_sql(ss.str());

        ss.str(""); 
        ss<<"update filemd5_count set count=count-1 where file_md5='"
             <<filemd5.c_str()<<"';";
        data->mydb.exec_sql(ss.str());
    }
    return 0;
}
