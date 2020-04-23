
#include <sys/timerfd.h>
#include <unistd.h>

#include <cassert>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <memory>
#include <thread>

#include "../network/net/Channel.h"
#include "../network/net/EventLoop.h"
#include "../network/net/Timestamp.h"
using std::cout;
using std::endl;
using std::shared_ptr;
using std::unique_ptr;
shared_ptr<EventLoop> g_loop;

void timeout(Timestamp t) {
    cout << t.toFormatString() << " Timeout!" << endl;
    g_loop->quit();
}

int main() {
    cout << Timestamp::now().toFormatString() << " started" << endl;
    g_loop.reset(new EventLoop{});
    g_loop->init();
    int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
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