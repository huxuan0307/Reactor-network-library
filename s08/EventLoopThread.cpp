
#include "EventLoopThread.h"

#include "EventLoop.h"
using std::bind;
using std::lock_guard;
using std::mutex;
using std::thread;
using std::unique_lock;
EventLoopThread::EventLoopThread() : exiting_{false} {}

EventLoopThread::~EventLoopThread()
{
    exiting_ = true;
    loop_->quit();
    thread_->join();
}

shared_ptr<EventLoop> EventLoopThread::startLoop() {
    thread_.reset(new thread(bind(&EventLoopThread::threadFunc, this)));
    {
        // lock_guard<mutex> lock(mutex_);
        if (!loop_) {
            unique_lock<mutex> ul (mutex_);
            cond_.wait(ul);
        }
    }
    return loop_;
}

void EventLoopThread::threadFunc() {
    {
        lock_guard<mutex> lock(mutex_);
        loop_.reset(new EventLoop());
        cond_.notify_one();
    }
    loop_->init();
    loop_->loop();
}
