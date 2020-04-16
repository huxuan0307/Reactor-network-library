#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <thread>
#include "../network/net/EventLoop.h"
#include <thread>
using std::cout;
using std::endl;

EventLoop* g_loop;

void threadFunc(){
    cout<<"threadFunc(), pid="<<getpid()<<", tid="<<std::this_thread::get_id()<<endl;
    g_loop->loop();
}

int main()
{
    cout<<"threadFunc(), pid="<<getpid()<<", tid="<<std::this_thread::get_id()<<endl;
    EventLoop loop;
    g_loop = &loop;
    std::thread th(threadFunc);
    loop.loop();
    th.join();
}