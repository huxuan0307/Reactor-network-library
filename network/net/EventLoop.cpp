
#include "EventLoop.h"
#include <cassert>
#include <cstdio>
#include <poll.h>
// v in one thread
thread_local EventLoop* t_loopInThisThread = nullptr;
EventLoop::EventLoop()
:looping_{false}, quit_{false}, threadId_{std::this_thread::get_id()}
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
    ::poll(nullptr, 0, 5*1000);
    looping_ = false;

}

EventLoop* getEventLoopOfCurrentThread(){
    return t_loopInThisThread;
}

