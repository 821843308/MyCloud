#include <iostream>
#include "server.h"

int main()
{
    Server server;
    server.start("0.0.0.0",9292);
    server.epoll_work();
    return 0;
}
