#include <iostream>
#include "client.h"
#include <signal.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "../Window/Window.h"
#include "../comm/transfer/transfer.h"
#include "../comm/UserData/UserData.h"
#include "../md5/md5.h"

Client *pclient;
pthread_t header,client_flist,server_flist,input_output;
std::vector<std::string> local_flist;
std::vector<std::string> icloud_flist;
pthread_cond_t cond1;
pthread_cond_t cond2;
pthread_mutex_t mutex1;
pthread_mutex_t mutex2;



void* run_header(void* arg)
{
    Window window ;
    window.init();
    std::string strheader = "Welcome to icloud system";
    window.draw_header();
    window.put_str_to_header(strheader);
}

int scanFiles(std::vector<std::string> &fileList, std::string inputDirectory)
{
    inputDirectory = inputDirectory.append("/");

    DIR *p_dir;
    const char* str = inputDirectory.c_str();

    p_dir = opendir(str);   
    if( p_dir == NULL )
    {
        std::cout<< "can't open :" << inputDirectory << std::endl;
    }

    struct dirent *p_dirent;

    while ( p_dirent = readdir(p_dir) )
    {
        std::string tmpFileName = p_dirent->d_name;
        if( tmpFileName == "." || tmpFileName == ".." )
            continue;
                                
        else
            fileList.push_back(tmpFileName);
                                
    }
        closedir(p_dir);
        return fileList.size();

}
void* run_client_flist(void* arg)
{
    Window window ;
    window.init();
    window.draw_client_flist();
    window.win_refresh(window.client_flist);
    int y,x;
    getmaxyx(window.client_flist,y,x);
    std::string dir = "../local/localfiles";

    scanFiles(local_flist,dir);
    while(1)
    {
    
        window.draw_client_flist();
        window.win_refresh(window.client_flist);
        int size = local_flist.size();
        std::string str;
        char buf[64];
        sprintf(buf,"本地文件总数:%d",size);
        str = buf;
        window.put_str_to_win(window.client_flist,1,1,str);
    
        for(int i = 0;i<size;i++)
        {
    
            sprintf(buf,"%d.",i+1);
            str = buf;
            window.put_str_to_win(window.client_flist,i+2,1,str);
            window.put_str_to_win(window.client_flist,i+2,1+str.size(),local_flist[i]);
        }
        
        window.win_refresh(window.client_flist);

        pthread_cond_wait(&cond2,&mutex2);
        local_flist.clear();
        scanFiles(local_flist,dir);
    
    }
}

void* run_server_flist(void* arg)
{
    Window window ;
    window.init();
    window.draw_server_flist();
    window.win_refresh(window.server_flist);
    while(1)
    {
        Window window ;
        window.init();
        window.draw_server_flist();
        window.win_refresh(window.server_flist);
    
        int size = icloud_flist.size();
        std::string str;
        char buf[64];
        sprintf(buf,"网盘文件总数:%d",size);
        str = buf;
        window.put_str_to_win(window.server_flist,1,1,str);
    
        for(int i = 0;i<size;i++)
        {
    
            sprintf(buf,"%d.",i+1);
            str = buf;
            window.put_str_to_win(window.server_flist,i+2,1,str);
            window.put_str_to_win(window.server_flist,i+2,1+str.size(),icloud_flist[i]);
        }
        
        window.win_refresh(window.server_flist);
    
        pthread_cond_wait(&cond1,&mutex1);
    }
}


void* run_input_output(void* arg)
{
    Window window ;
    window.init();
    window.draw_input();
    window.win_refresh(window.input);
    window.draw_output();
    window.win_refresh(window.output);

    int input_y,input_x;
    getmaxyx(window.input,input_y,input_x);
    
    int output_y,output_x;
    getmaxyx(window.output,output_y,output_x);

    int line = 1;

    std::string tips;
    std::string str;

    std::string jsonstring;

    while(1)
    {
        pclient->user_data_.cmd = "";
        pclient->user_data_.cmd_state = "";
        pclient->user_data_.msg = "";

        /* window.draw_input(); */
        /* window.win_refresh(window.input); */
        /* window.draw_output(); */
        /* window.win_refresh(window.output); */

        tips = "1.上传   2.下载   3.删除";
        window.put_str_to_win(window.input,1,(input_x-tips.size())/2,tips);
        window.win_refresh(window.input);
    
        
        std::string msg;
        
        while(msg != "1" && msg != "2" && msg != "3")
        {
            str= "请输入命令编号：";
            window.clear_win_line(window.input,input_y/2,1);
            window.put_str_to_win(window.input,input_y/2,1,str);
            window.win_refresh(window.input);
            window.get_str_from_win(window.input,msg);
        }

        if(msg == "1")
        {
            std::string filename;
            std::string num;
            tips = "上传";
            while(filename.empty())
            {
                window.draw_input();
                window.put_str_to_win(window.input,1,(input_x-tips.size())/2,tips);
                window.win_refresh(window.input);
                str = "请输入上传文件编号：";
                window.put_str_to_win(window.input,input_y/2,1,str);
                window.win_refresh(window.input);
                window.get_str_from_win(window.input,num);
                
                if(line == output_y-1)
                {
                    window.draw_output();
                    window.win_refresh(window.output);
                    line = 1;
                }
                //判断文件编号是否合法，合法判断icloud_flist是否存在该文件名
                if(!num.empty() && atoi(num.c_str())> 0 && atoi(num.c_str())<= local_flist.size())
                {
                    filename = local_flist[atoi(num.c_str())-1];
                    
                    //icloud_flist不为空才看存不存在
                    if(!icloud_flist.empty())
                    {
                        int i = 0;
                        while(i<icloud_flist.size())
                        {
                            if((strcmp(filename.c_str(),icloud_flist[i].c_str())) == 0)
                                break;
                            else
                                i++;
                        }
                        if(i != icloud_flist.size())//存在相同文件名,上传文件名不合法，重输
                        {
                            std::string output_str;
                            output_str = filename+"文件名已存在！";
                            window.put_str_to_win(window.output,line++,1,output_str);
                            window.win_refresh(window.output);

                            filename ="";
                        }
                    }
                }
            }
        
            //通知服务器要接收的文件属性
            //发送msg编号的文件给服务器
            
            if(line >= output_y-3)
            {
                window.draw_output();
                window.win_refresh(window.output);
                line = 1;
            }

            std::string output_str;
            output_str = filename;
            output_str = "正在上传："+output_str+"...";
            window.put_str_to_win(window.output,line++,1,output_str);
            window.win_refresh(window.output);

            char path[50];
            sprintf(path,"../local/localfiles/%s",filename.c_str());

            int ret = pclient->upload(path,filename.c_str());
            /* sleep(5); */
            window.win_refresh(window.output);

            if(ret < 0)//-1 出错，0成功，1没完成
            {
                
            }
            else if(ret == 0)
            {
                window.put_str_to_win(window.output,line++,1,pclient->user_data_.msg);
                window.win_refresh(window.output); 

                icloud_flist.push_back(filename);
                pthread_cond_signal(&cond1);
            }

        }
        else if(msg == "2")
        {
            std::string num;
            tips = "下载";
            while(num.empty()||atoi(num.c_str())<= 0||atoi(num.c_str())>icloud_flist.size())
            {
                window.draw_input();
                window.put_str_to_win(window.input,1,(input_x-tips.size())/2,tips);
                window.win_refresh(window.input);

                str = "请输入下载文件编号：";
                window.put_str_to_win(window.input,input_y/2,1,str);
                window.win_refresh(window.input);
                window.get_str_from_win(window.input,num);

            }

            //通知服务器要下载的文件
            //接收文件属性
            //接收文件
            char filename[20];
            sprintf(filename,"%s",(icloud_flist[atoi(num.c_str())-1]).c_str());
            std::string output_str;
            output_str = filename;
            output_str = "正在下载："+output_str+"...";
            window.put_str_to_win(window.output,line++,1,output_str);
            window.win_refresh(window.output);

            int ret = pclient->download(filename);
            if(ret < 0)//-1 出错，0成功，1没完成
            {
                
            }
            else if(ret == 0)
            {
                std::string str;
                str = "下载成功！";
                window.put_str_to_win(window.output,line++,1,str);
                window.win_refresh(window.output); 

                pthread_cond_signal(&cond2);
            }


        }
        else if(msg == "3")
        {
            std::string num;
            tips = "删除";
            while(num.empty()||atoi(num.c_str())<= 0||atoi(num.c_str())>icloud_flist.size())
            {
                window.draw_input();
                window.put_str_to_win(window.input,1,(input_x-tips.size())/2,tips);
                window.win_refresh(window.input);

                str = "请输入删除文件编号：";
                window.put_str_to_win(window.input,input_y/2,1,str);
                window.win_refresh(window.input);
                window.get_str_from_win(window.input,num);
                if(atoi(num.c_str())<= 0||atoi(num.c_str())>icloud_flist.size())
                {
                    if(line >= output_y-1)
                    {
                        window.draw_output();
                        window.win_refresh(window.output);
                        line = 1;
                    }
                    std::string str;
                    str = "删除的文件不存在！请重新输入！";
                    window.put_str_to_win(window.output,line++,1,str);
                    window.win_refresh(window.output); 

                }

            }

            //通知服务器要删除的文件
            //删除server_flist内的该项

            if(line >= output_y-3)
            {
                window.draw_output();
                window.win_refresh(window.output);
                line = 1;
            }
            char filename[20];
            sprintf(filename,"%s",(icloud_flist[atoi(num.c_str())-1]).c_str());
            std::string output_str;
            output_str = filename;
            output_str = "正在删除："+output_str+"...";
            window.put_str_to_win(window.output,line++,1,output_str);
            window.win_refresh(window.output);
            
            int ret =  pclient->deletefile(filename);
            /* if(ret == 0) */
            /* { */
            /*     std::string str; */
            /*     str = "删除成功！"; */
            /*     window.put_str_to_win(window.output,line++,1,str); */
            /*     window.win_refresh(window.output); */ 

            /*     icloud_flist.erase(icloud_flist.begin()+atoi(num.c_str())-1); */
            /*     pthread_cond_signal(&cond1); */

            /* } */
            window.put_str_to_win(window.output,line++,1,pclient->user_data_.msg);
            window.win_refresh(window.output);
            if(ret == 0)
            {
                icloud_flist.erase(icloud_flist.begin()+atoi(num.c_str())-1);
                pthread_cond_signal(&cond1);

            }
        }

    }
    

}

void sendQuit(int sig)
{
    pthread_cancel(header);
    pthread_cancel(client_flist);
    pthread_cancel(server_flist);
    pthread_cancel(input_output);
}


void get_icloud_flist(std::string &msg)
{
    int size = msg.size();
    std::string filename;
    for(int i = 0;i<size;i++)
    {
        if(msg[i] != ' ')
        {
            filename +=msg[i];
        }
        else
        {
           icloud_flist.push_back(filename);
           filename ="";
           
        }
    }

}
void usage(const char* proc)
{
    std::cout<<"usage:\n\t"<<proc<<"[server_ip]"<<"[server_port]\r\n"<<std::endl;
}

int main(int argc,char*argv[])
{

    if(argc != 3)
    {
        usage(argv[0]);
        return 1;
    }

    Client client;
    client.init(argv[1],atoi(argv[2]));

    pthread_cond_init(&cond1,NULL);
    pthread_cond_init(&cond2,NULL);
    pthread_mutex_init(&mutex1,NULL);
    pthread_mutex_init(&mutex2,NULL);
    
    pclient = &client;

    Window window;
    window.init();
    
    window.draw_output();
    client.choose_cmd(&window);

    if(client.user_data_.cmd == "sign_in")//登陆
    {
        client.login(&window);
    }
    else if(client.user_data_.cmd == "sign_up")//注册
    {
        client.regist(&window);
        
    }
    else//退出
    {
        return 0;
    }


    while(client.user_data_.cmd_state != "S")
    {
       if(client.user_data_.cmd == "sign_in")
       {
            client.user_data_.cmd_state = "";
            client.user_data_.id = "";
            client.user_data_.passwd ="";
            client.login(&window);
       }
       else if(client.user_data_.cmd == "sign_up")
       {
            client.user_data_.cmd_state = "";
            client.user_data_.id = "";
            client.user_data_.passwd ="";
            client.regist(&window);
       }
    }

    //成功msg内是文件列表，以空格为分格
    get_icloud_flist(client.user_data_.msg);
    
    client.user_data_.cmd = "";
    client.user_data_.cmd_state = "";
    client.user_data_.msg = "";

    signal(SIGINT,sendQuit);
/* pthread_t header,client_flist,output,server_flist,input; */

    pthread_create(&header,NULL,run_header,NULL);
    pthread_create(&client_flist,NULL,run_client_flist,NULL);
    pthread_create(&server_flist,NULL,run_server_flist,NULL);
    pthread_create(&input_output,NULL,run_input_output,NULL);

    pthread_join(header,NULL);
    pthread_join(client_flist,NULL);
    pthread_join(server_flist,NULL);
    pthread_join(input_output,NULL);

    pthread_mutex_destroy(&mutex1);
    pthread_mutex_destroy(&mutex2);
    pthread_cond_destroy(&cond1);
    pthread_cond_destroy(&cond2);
    
    return 0;
}
