#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <iostream>
#include <ncurses.h>
#include <string.h>
#include <unistd.h>
#include <locale.h>

#define MAX 1024

class Window
{
public:
    Window();
    void init();
    WINDOW* creat_win(const int &h,const int &w,const int &y,const int &x);
    void win_refresh(WINDOW *_win);
    void put_str_to_header(std::string &_str);
    void put_str_to_win(WINDOW *_win,int y,int x,std::string &_str);

    void get_str_from_win(WINDOW *_win,std::string &_str);
    void clear_win_line(WINDOW *_win,int begin,int num);

    ~Window();

    void draw_header();
    void draw_client_flist();
    void draw_output();
    void draw_server_flist();
    void draw_input();

public:
    WINDOW *header;
    WINDOW *client_flist;
    WINDOW *output;
    WINDOW *server_flist;
    WINDOW *input;
};
#endif
