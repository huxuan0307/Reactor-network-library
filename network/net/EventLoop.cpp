
#include "TimerQueue.h"
#include "Channel.h"
#include "Poller.h"
#include "EventLoop.h"
#include "TimerId.h"
#include <cassert>
#include <cstdio>
#include <poll.h>
#include <sys/eventfd.h>
#include <iostream>
#include <mutex>
#include <thread>
#include <unistd.h>
using std::cerr;
using std::cout;
using std::endl;

// v in one thread
thread_local EventLoop* t_loopInThisThread = nullptr;
const int EventLoop::kPOllerTime_ms = 10000;    // 1000ms

static int createEventfd(){
    // fd for event
    int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if(evtfd < 0){
        cerr<<"Failed in eventfd";
        abort();
    }
    return evtfd;
}

EventLoop::EventLoop()
    : looping_{false},
      quit_{false},
      threadId_{std::this_thread::get_id()},
      wakeupFd_{createEventfd()} 
    {
    if(t_loopInThisThread){
        // error this_thread already has a EventLoop
        FATAL << "this_thread already has a EventLoop";
        assert(false);
    }
    else{
        t_loopInThisThread = this;
    }
}

void EventLoop::init()
{
    poller_.reset(new Poller(shared_from_this()));
    timerQueue_.reset(new TimerQueue{shared_from_this()});
    wakeupChannel_.reset(new Channel(shared_from_this(), wakeupFd_));
    wakeupChannel_->setReadCallback(std::bind(&EventLoop::handleRead, this));
    wakeupChannel_->enableReading();
}

EventLoop::~EventLoop(){
    TRACE << "~EventLoop()";
    assert(!looping_);
    t_loopInThisThread = nullptr;
}

void EventLoop::loop(){
    TRACE << "EventLoop::loop()";
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
            assert(channel.lock());
            TRACE << "active channel index: " << channel.lock()->index();
            if(channel.lock())
                channel.lock()->handleEvent(Timestamp::now());
        }
        doPendingFunctors();
        TRACE << "current pollfdCnt: " << poller_->pollfdCnt();
    }
    looping_ = false;
}

void EventLoop::quit(){
    TRACE << "EventLoop::quit()";
    // set quit flag, active at next loop in EventLoop::loop()
    quit_ = true;
    if(!isInLoopThread()){
        wakeup();
    }
}

void EventLoop::runInLoop(const Event& cb)
{
    TRACE << "EventLoop::runInLoop()";
    if(isInLoopThread()){
        cb();
    }else{
        queueInLoop(cb);
    }
}

void EventLoop::queueInLoop(const Event& cb)
{
    TRACE<<"EventLoop::queueInLoop()";
    {
        std::lock_guard<std::mutex> lg(mutex_);
        pendingFunctors_.push_back(cb);
    }
    if(!isInLoopThread() || callingPendingFunctors_){
        wakeup();
    }
}

void EventLoop::abortNotInLoopThread(){
    Fatal() << "is not in loop thread";
    std::cerr<<"is not in loop thread"<<std::endl;
    abort();
}

EventLoop* getEventLoopOfCurrentThread(){
    return t_loopInThisThread;
}

void EventLoop::updateChannel(std::shared_ptr<Channel> channel){
    TRACE<<"EventLoop::updateChannel()";
    assert(channel->ownerLoop().lock().get() == this);
    assertInLoopThread();
    poller_->updateChannel(channel);
}

void EventLoop::removeChannel(std::shared_ptr<Channel> channel){
    TRACE<<"EventLoop::removeChannel()";
    assert(channel->ownerLoop().lock().get() == this);
    assertInLoopThread();
    poller_->removeChannel(channel);
}

TimerId EventLoop::run_at(const Timestamp& timestamp, const TimerCallback& cb)
{
    return timerQueue_->addTimer(cb, timestamp, 0.0);
}

TimerId EventLoop::run_after(double delay, const TimerCallback& cb)
{
    Timestamp t{Timestamp::now()};
    t+=delay;
    return run_at(t, cb);
}

TimerId EventLoop::run_every(double interval, const TimerCallback& cb)
{
    Timestamp t{Timestamp::now()};
    t+=interval;
    return timerQueue_->addTimer(cb, t, interval);
}

void EventLoop::cancelTimer(TimerId timerid){
    timerQueue_->cancel(timerid);
}

// use IO to weakup EventLoop
void EventLoop::wakeup()
{
    TRACE<<"EventLoop::wakeup()";

    uint64_t one = 1; //?
    auto ret = ::write(wakeupFd_, &one, sizeof one);
    if(ret != sizeof one){
        cerr << "EventLoop::wakeup() write " << ret << " Bytes instead of 8"
             << endl;
    }
}

size_t EventLoop::pollfdCnt(){return poller_->pollfdCnt();}


void EventLoop::handleRead()
{
    TRACE<<"EventLoop::handleRead()";
    
    uint64_t one = 1;
    auto ret = ::read(wakeupFd_, &one, sizeof one);
    if (ret != sizeof one) {
        cerr << "EventLoop::handleRead() reads " << ret
             << " bytes instead of 8"<<endl;
    }
}

void EventLoop::doPendingFunctors()
{
    TRACE<<"EventLoop::doPendingFunctors()";

    std::vector<Event> functors;
    callingPendingFunctors_ = true;
    {
        std::lock_guard<std::mutex> lg(mutex_);
        functors.swap(pendingFunctors_);
    }
    for(auto& event: functors){
        event();
    }
    callingPendingFunctors_ = false;
    TRACE<<"EventLoop::doPendingFunctors() done!";
}