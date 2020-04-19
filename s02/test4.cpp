
#include "Timestamp.h"
#include "EventLoop.h"
#include "TimerId.h"
#include "TimerQueue.h"
#include <cstdio>
#include <thread>
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <memory>
#include <algorithm>
using namespace std;

int cnt = 0;
shared_ptr<EventLoop> g_loop;

void print_tid()
{
    cout<<"pid = "<<getpid()<<", tid = "<<this_thread::get_id()<<endl;
    cout<<"now "<< Timestamp::now().toString()<<endl;
}

void print_msg(const char* msg)
{
    printf("msg %s %s\n", Timestamp::now().toString().c_str(), msg);
    // g_loop->timerQueue_->checkTimer();
    if(++::cnt==20){
        g_loop->quit();
    }
}

int main()
{
    print_tid();
    g_loop.reset(new EventLoop{});
    // print_msg("main");
    g_loop->init();
    // g_loop->run_after(1, bind(print_msg, "once 1"));
    // g_loop->run_after(1.5, bind(print_msg, "once 1.5"));
    // g_loop->run_after(6, bind(print_msg, "once 6"));
    // g_loop->run_after(3, bind(print_msg, "once 3"));
    // g_loop->run_after(2, bind(print_msg, "once 2"));
    g_loop->run_every(1, bind(print_msg, "every 1"));
    g_loop->run_every(2, bind(print_msg, "every 2"));
    g_loop->run_every(3, bind(print_msg, "every 3"));

    // g_loop->timerQueue_->checkTimer();
    g_loop->loop();
    print_msg("main loop exits");
    // sleep(1);
}
