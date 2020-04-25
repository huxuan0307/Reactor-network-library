
#include "EventLoopThreadPool.h"

using namespace std;

EventLoopThreadPool::EventLoopThreadPool(weak_ptr<EventLoop> loop, size_t threadNum)
:baseloop_{loop},
threadNum_{threadNum},
started_{false},
nextThreadIndex_{0}
{}

EventLoopThreadPool::~EventLoopThreadPool()
{

}

void EventLoopThreadPool::start()
{
    assert(!started_);
    baseloop_.lock()->assertInLoopThread();
    started_ = true;

    for(size_t i=0;i<threadNum_;++i){
        shared_ptr<EventLoopThread> t{new EventLoopThread{}};
        eventLoopThreads_.push_back(t);     // loopthreads contains loop
        loops_.push_back(t->startLoop());   // contains weak ref to loop
    }
}

weak_ptr<EventLoop> EventLoopThreadPool::getNextLoop()
{
    baseloop_.lock()->assertInLoopThread();
    weak_ptr<EventLoop> loop;
    if(!loops_.empty()){
        loop = loops_[getNextLoopIndex()];
    }else{
        loop = baseloop_;
    }
    return loop;
}

