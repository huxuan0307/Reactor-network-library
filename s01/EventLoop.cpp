
#include "Channel.h"
#include "Poller.h"
#include "EventLoop.h"
#include <cassert>
#include <cstdio>
#include <poll.h>
#include <iostream>

using std::cerr;
using std::cout;
using std::endl;

// v in one thread
thread_local EventLoop* t_loopInThisThread = nullptr;
const int EventLoop::kPOllerTime_ms = 1000;    // 1000ms
EventLoop::EventLoop()
:looping_{false}, quit_{false}, threadId_{std::this_thread::get_id()},
poller_{new Poller(shared_from_this())}
{
    if(t_loopInThisThread){
        // error this_thread already has a EventLoop
        fprintf(stderr, "this_thread already has a EventLoop\n");
    }
    else{
        t_loopInThisThread = this;
    }
}

EventLoop::~EventLoop(){
    assert(!looping_);
    t_loopInThisThread = nullptr;
}

void EventLoop::loop(){
    assert(!looping_);

    looping_ = true;
    quit_ = false;
    while (!quit_)
    {
        activeChannels_.clear();
        // get active channels
        poller_->poll(kPOllerTime_ms, activeChannels_ );
        // handle active channels
        for(auto& channel: activeChannels_){
            channel.lock()->handleEvent();
        }
    }
    looping_ = false;
}

void EventLoop::quit(){
    // set quit flag, active at next loop in EventLoop::loop()
    quit_ = true;
}

void EventLoop::abortNotInLoopThread(){
    std::cerr<<"is not in loop thread"<<std::endl;
    abort();
}

EventLoop* getEventLoopOfCurrentThread(){
    return t_loopInThisThread;
}

void EventLoop::updateChannel(std::weak_ptr<Channel> channel){
    cerr<<"EventLoop::updateChannel"<<endl;
    assert(channel.lock()->ownerLoop().lock().get() == this);
    assertInLoopThread();
    poller_->updateChannel(channel.lock());
}