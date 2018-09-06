#include "client.h"
#include <fstream>
#include <sys/sendfile.h>
#include "../comm/transfer/transfer.h"
#include "../comm/UserData/UserData.h"

int Client::init(const std::string& server_ip,short server_port)
{
    sock_=socket(AF_INET,SOCK_STREAM,0);
    if(sock_<0){
        perror("error:socket");
        return -1;
    }

    server_addr_.sin_family=AF_INET;
    server_addr_.sin_addr.s_addr=inet_addr(server_ip.c_str());
    server_addr_.sin_port=htons(server_port);
    
    int ret=connect(sock_,(sockaddr*)&server_addr_,sizeof(server_addr_));
    if(ret<0){
        perror("error:connect");
        return -1;
    }
    return 0;
}

void show_choose(Window* pwindow)
{
    pwindow->draw_output();
    int y,x;
    getmaxyx(pwindow->output,y,x);
    std::string str0 = "Please Choose";
    std::string str1 = "1. Sign in ";
    std::string str2 = "2. Sign up ";
    std::string str3 = "3.  Quit   ";
    pwindow->put_str_to_win(pwindow->output,(y-9)/2,(x-str0.size())/2,str0);
    pwindow->put_str_to_win(pwindow->output,(y-9)/2+3,(x-str1.size())/2,str1);
    pwindow->put_str_to_win(pwindow->output,(y-9)/2+5,(x-str2.size())/2,str2);
    pwindow->put_str_to_win(pwindow->output,(y-9)/2+7,(x-str3.size())/2,str3);
    pwindow->win_refresh(pwindow->output);

}
void Client::choose_cmd(Window* pwindow)
{
    pwindow->draw_output();
    int y,x;
    getmaxyx(pwindow->output,y,x);
    std::string cmd;

    keypad(stdscr,TRUE);
    int key;
    MEVENT event;
    int Y,X;
    mousemask(BUTTON1_CLICKED,0);
    while(1)
    {
        show_choose(pwindow);
        key = getch();
        if(key == KEY_MOUSE)
        {
            getmouse(&event);
            Y = event.y;
            X = event.x;
            if(wenclose(pwindow->output,Y,X))
            {
                wmouse_trafo(pwindow->output,&Y,&X,false);
                if(Y == ((y-9)/2+3)) 
                {

                    user_data_.cmd = "sign_in" ;
                    break;
                } 
                else if(Y == ((y-9)/2+5))
                {
                    user_data_.cmd = "sign_up" ;
                    break;
                }
                else if(Y == ((y-9)/2+7))
                {
                    user_data_.cmd = "quit";
                    break;
                }
            }
        }
    }

    mousemask(0,0);
    

}

void Client::login(Window* pwindow)
{
    pwindow->draw_output();
    int y,x;
    getmaxyx(pwindow->output,y,x);

    std::string error_msg = user_data_.msg;
    user_data_.msg = "";
    if(!error_msg.empty())
    {
        pwindow->put_str_to_win(pwindow->output,2,(x-error_msg.size())/2,error_msg);
    }
    std::string str;
    str= "Please Enter Id: ";
    pwindow->put_str_to_win(pwindow->output,y/2-1,x/2-str.size(),str);
    pwindow->win_refresh(pwindow->output);
    pwindow->get_str_from_win(pwindow->output,user_data_.id);

    pwindow->draw_output();
    str= "Please Enter Password: ";
    pwindow->put_str_to_win(pwindow->output,y/2-1,x/2-str.size(),str);
    pwindow->win_refresh(pwindow->output);
    std::string passwd;
    pwindow->get_str_from_win(pwindow->output,passwd);

    md5.handle_str(passwd);
    user_data_.passwd = md5.md5_string();

    std::string outdata;
    std::string indata;

    user_data_.serialize(outdata);
    Transfer::set_data(sock_,outdata);
    Transfer::get_data(sock_,&indata);
    user_data_.unserialize(indata);
}

void Client::regist(Window* pwindow)
{
    pwindow->draw_output();
    int y,x;
    getmaxyx(pwindow->output,y,x);
    std::string tips;
    tips = "Id must is phone number!";
    std::string error_msg = user_data_.msg;
    user_data_.msg = "";
    while(user_data_.id.size() != 11)
    {
        pwindow->draw_output();
        if(!error_msg.empty())
        {
            pwindow->put_str_to_win(pwindow->output,2,(x-error_msg.size())/2,error_msg);
        }
        pwindow->win_refresh(pwindow->output);
        pwindow->put_str_to_win(pwindow->output,y/2-5,(x-tips.size())/2,tips);
        std::string str;
        str= "Please Enter Id: ";
        pwindow->put_str_to_win(pwindow->output,y/2-1,x/2-tips.size(),str);
        pwindow->win_refresh(pwindow->output);
        pwindow->get_str_from_win(pwindow->output,user_data_.id);

    }

    std::string passwd;
    std::string passwdtmp = "";
    while(passwdtmp.empty() || passwdtmp != passwd)
    {
        pwindow->draw_output();
        pwindow->win_refresh(pwindow->output);
        tips = "Please Enter Password: ";
        pwindow->put_str_to_win(pwindow->output,y/2-1,x/2-tips.size(),tips);
        pwindow->win_refresh(pwindow->output);
        pwindow->get_str_from_win(pwindow->output,passwdtmp);

        pwindow->clear_win_line(pwindow->output,y/2-1,1);
        
        tips = "Please Enter Password Again: ";
        pwindow->put_str_to_win(pwindow->output,y/2-1,1,tips);
        pwindow->win_refresh(pwindow->output);
        pwindow->get_str_from_win(pwindow->output,passwd);

    }
    md5.handle_str(passwd);
    user_data_.passwd = md5.md5_string();
    std::string outdata;
    std::string indata;

    user_data_.serialize(outdata);
    Transfer::set_data(sock_,outdata);
    Transfer::get_data(sock_,&indata);
    user_data_.unserialize(indata);
}


int Client::upload(const char* path,const char* filename)
{
    //通知服务器要接收的文件属性
    //发送msg编号的文件给服务器
    struct stat sta;
    if(stat(path,&sta)<0)
    {
        
    }
    else
    {
        std::string filemd5;

        std::ifstream in;
        in.open(path,std::ifstream::in);
        md5.handle_file(in);
        filemd5=md5.md5_string();

        std::string upload_msg;
        Json::Value value;
        value["filename"]=filename;
        value["filesize"] =(double)sta.st_size;
        value["filemd5"] = filemd5;

        Transfer::valueTostr(value,&upload_msg);
        user_data_.cmd = "upload";
        user_data_.msg =upload_msg;

        std::string outdata;
        std::string indata;

        user_data_.serialize(outdata);
        Transfer::set_data(sock_,outdata);
        Transfer::get_data(sock_,&indata);
        user_data_.unserialize(indata);

        if(user_data_.cmd_state =="S")//秒传
            return 0;
        else if(user_data_.cmd_state == "Please Send")
        {
            int fd = open(path,O_RDONLY);
            if(fd < 0)
            {
                /* std::cout<<"open error"<<std::endl; */
               return -2;
            }

            //看偏移量
            int sz = sendfile(sock_,fd,NULL,sta.st_size);
            if(sz < 0)
                return -1;

            else
                return 0;
            /* else */
            /*     return 1;//没发完 */


        }

    }

}
    
int Client::download(const char*filename)
{
    user_data_.cmd = "download";
    user_data_.msg =filename;
    std::string outdata;
    std::string indata;

    user_data_.serialize(outdata);
    Transfer::set_data(sock_,outdata);


    std::string path;
    path = filename;
    path = "../local/localfiles/"+path;
    int fd;
    int i = 1;
    char end[5] ="";
    //循环打开，直到打不开，打不开说明没有这个文件，文件名可用
    while((fd =open((path+end).c_str(),O_RDONLY)) > 0)
    {
        close(fd);
        sprintf(end,"(%d)",i++);
    }
    path = path+end;
    fd = open(path.c_str(),O_CREAT|O_RDWR,0666);
    if(fd == -1)
    {
        /* std::cout<<"open fd error"<<std::endl; */
    }

    char buf[1024];
    int rdsz = -1;
    while((rdsz = recv(sock_,buf,64,0)) >0)
    {
        buf[rdsz] = '\0';

        std::cout<<buf<<std::endl;
        int wrsz = write(fd,buf,rdsz);
        if(wrsz != rdsz)
        {
            /* std::cout<<"writ false"<<std::endl; */
            return -1;
        }
        if(rdsz < 64)
            break;

    }
    close(fd);

    return 0;
    
    
    
}
int Client::deletefile(const char*filename)
{
    user_data_.cmd = "deletefile";
    user_data_.msg =filename;
    std::string outdata;
    std::string indata;

    user_data_.serialize(outdata);
    Transfer::set_data(sock_,outdata);
    Transfer::get_data(sock_,&indata);
    user_data_.unserialize(indata);
    if(user_data_.cmd_state == "S")
    {
        return 0;
    }
    else
        return -1;


}
