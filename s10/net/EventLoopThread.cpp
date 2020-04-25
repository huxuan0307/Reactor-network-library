
#include "EventLoopThread.h"

#include "EventLoop.h"
#include <thread>
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
    // set logfile here
    static size_t subthreadnum = 0;
    MessageLogger::setLogPostfix(string("-sub")+std::to_string(++subthreadnum));
    {
        lock_guard<mutex> lock(mutex_);
        loop_.reset(new EventLoop());
        cond_.notify_one();
    }
    loop_->init();  // init here
    loop_->loop();
}
