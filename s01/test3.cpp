#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <thread>
#include "../network/net/EventLoop.h"
#include "../network/net/Channel.h"
#include <thread>
#include <sys/timerfd.h>
#include <memory>
#include <cstring>
using std::cout;
using std::endl;
using std::unique_ptr;
using std::shared_ptr;
shared_ptr<EventLoop> g_loop;

void timeout(){
    printf("Timeout!\n");
    g_loop->quit();
}

int main()
{
    g_loop.reset(new EventLoop{});
    int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK|TFD_CLOEXEC);
    shared_ptr<Channel> channel{new Channel(g_loop, timerfd)};
    channel->setReadCallback(timeout);
    channel->enableReading();
    itimerspec howlong;
    bzero(&howlong, sizeof howlong);
    howlong.it_value.tv_sec = 5;
    ::timerfd_settime(timerfd, 0, &howlong, nullptr);
    g_loop->loop();
    ::close(timerfd);
    
}