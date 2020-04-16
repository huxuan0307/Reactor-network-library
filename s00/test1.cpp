#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <thread>
#include "../network/net/EventLoop.h"
#include <thread>
using std::cout;
using std::endl;
void threadFunc(){
    cout<<"threadFunc(), pid="<<getpid()<<", tid="<<std::this_thread::get_id()<<endl;

    EventLoop loop;
    loop.loop();
}

int main()
{
    cout<<"threadFunc(), pid="<<getpid()<<", tid="<<std::this_thread::get_id()<<endl;

    EventLoop loop;
    std::thread th(threadFunc);
    loop.loop();
    th.join();
}