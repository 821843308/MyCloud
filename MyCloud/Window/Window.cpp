#include "Window.h"

Window::Window()
    :header(NULL),client_flist(),output(NULL),server_flist(NULL),input(NULL)
{}
void Window::init()
{
    setlocale(LC_ALL,"");
    initscr();
    /* keypad(stdscr,TRUE); */
    curs_set(0);
}

WINDOW* Window::creat_win(const int &h,const int &w,const int &y,const int &x)
{
    WINDOW* win = newwin(h,w,y,x);
    return win;
}

void Window::win_refresh(WINDOW *_win)
{
    box(_win,0,0);
    wrefresh(_win);
}

void Window::clear_win_line(WINDOW *_win,int begin,int num)
{
    while(num--)
    {
        wmove(_win,begin++,0);
        wclrtoeol(_win);//将当前位置到窗口低端的所有字符清除
    }
}

void Window::put_str_to_header(std::string &_str)
{
    int step = 5;
    int y,x;
    getmaxyx(header,y,x);
    while(1)
    {
        draw_header();
        mvwaddstr(header,y/2,step++,_str.c_str());
        wrefresh(header);
        step %=(COLS-5-_str.size());
        if(step == 0)
            step = 5;
        sleep(1);

        wmove(header,y/2,1);
        wclrtoeol(header);//将当前位置到这一行末所以字符全清除
        /* wclrtobot(this->header);//将当前位置到窗口低端的所有字符清除 */
        box(header,0,0);
    }
}

void Window::put_str_to_win(WINDOW *_win,int y,int x,std::string &_str)
{
    mvwaddstr(_win,y,x,_str.c_str());
    /* mvwaddstr(_win,y,x,"hajksaj"); */

    wrefresh(_win);
}

void Window::get_str_from_win(WINDOW *_win,std::string&_out)
{
    char buf[MAX];
    memset(buf,'\0',sizeof(buf));
    wgetnstr(_win,buf,sizeof(buf));
    _out = buf;

}

Window::~Window()
{
    delwin(this->header);
    endwin();
}

void Window::draw_header()
{
    int h = LINES/10;
    int w = COLS;
    int y = 0;
    int x = 0;
    this->header = creat_win(h,w,y,x);
    this->win_refresh(header);
}


void Window::draw_client_flist()
{
    int h = 9*LINES/10;
    int w = COLS/4;
    int y = LINES/10;
    int x = 0;
    this->client_flist = creat_win(h,w,y,x);
    this->win_refresh(client_flist);
}

void Window::draw_output()
{
    int h = 3*LINES/5;
    int w = 2*COLS/4;
    int y = LINES/10;
    int x = COLS/4;
    this->output = creat_win(h,w,y,x);
    this->win_refresh(output);

}


void Window::draw_server_flist()
{
    int h = 9*LINES/10;
    int w = COLS/4;
    int y = LINES/10;
    int x = 3*COLS/4;
    this->server_flist = creat_win(h,w,y,x);
    this->win_refresh(server_flist);

    
}
void Window::draw_input()
{
    int h = 3*LINES/10;
    int w = 2*COLS/4;
    int y = 7*LINES/10;
    int x = COLS/4;
    this->input = creat_win(h,w,y,x);
    this->win_refresh(this->input);
}


/* int main() */
/* { */
/*     Window _win; */
/*     _win.init(); */

/*     _win.draw_header(); */
/*     /1* sleep(1); *1/ */
/*     _win.draw_client_flist(); */
/*     _win.draw_output(); */
/*     /1* sleep(1); *1/ */
/*     _win.draw_server_flist(); */
/*     _win.draw_input(); */
/*     /1* sleep(1); *1/ */
/*     /1* sleep(1); *1/ */

/*     std::string str; */
/*     str = "Welcome icloud system"; */

/*     _win.put_str_to_header(str); */ 
/*     /1* sleep(1); *1/ */

/*     return 0; */
/* } */
