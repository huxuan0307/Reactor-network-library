

#include "TimerId.h"
#include "Channel.h"
#include "TimerQueue.h"
#include "Timer.h"
#include <unistd.h>
#include <sys/timerfd.h>
#include <iostream>
#include <cassert>
using std::cerr;
using std::endl;
using std::cout;
// create a nonblock and closeonexec timerfd using timerfd_create()
int createTimerfd()
{
    int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    if(timerfd < 0){
        perror("timerfd_create");
    }
    return timerfd;
}

timespec timeDiffFromNow(Timestamp when)
{
    // difference between timespec and timeval is that timespec uses second and nanosecond
    // while timeval uses second and microsecond
    timespec res{0,0};
    auto timediff = when - Timestamp::now();
    if (timediff.valid()) {
        res.tv_sec = timediff.s_since_epoch();
        res.tv_nsec = timediff.getMsOnly() * 1000;
    }
    return res;
}

void showTimerfd(int timerfd, Timestamp now)
{
    uint64_t cnt;
    auto n = ::read(timerfd, &cnt, sizeof cnt);
    std::cout << "TimerQueue::handleRead() " << cnt << "at" << now.toString()<<endl;
    if(n!=sizeof cnt){
        cerr<<"TimerQueue::handleRead() reads "<<n<<" bytes instead of 8"<<endl;
    }
}


void resetTimerfd(int timerfd, Timestamp expiration)
{
    itimerspec newval;
    itimerspec oldval;
    bzero(&newval, sizeof newval);
    bzero(&oldval, sizeof oldval);
    newval.it_value = timeDiffFromNow(expiration);
    int ret = ::timerfd_settime(timerfd, 0, &newval, &oldval);
    if(ret){
        perror("timerfd_settime");
        cout<<expiration.toString()<<endl;
        cout<<newval.it_value.tv_sec<<":"<<newval.it_value.tv_nsec<<endl;
    }
}

TimerQueue::TimerQueue(weak_ptr<EventLoop> loop)
:loop_{loop},
timerfd_{createTimerfd()},
timerfdChannel_(new Channel(loop, timerfd_))
{
    assert(loop_.lock());
    loop_.lock()->assertInLoopThread();
    timerfdChannel_->setReadCallback(std::bind(&TimerQueue::handleRead, this));
    // cannot call Channel::enableReading here since Channel will use 
    timerfdChannel_->enableReading();
}

TimerQueue::~TimerQueue()
{
    // 此处disableAll意在把Poller里的注册的事件清空，但是由于架构的顺序，EventLoop的析构先于其成员变量TimerQueue，
    // 于是调用disableAll之前，EventLoop已经不复存在
    // 暂定的解决方式为：在disableAll的调用函数Channel::update中添加判断：loop->lock()
    // 即由weak_ptr尝试获得EventLoop的管理权，如果不存在，那么Poller和EventLoop是组合关系，
    // Poller也将析构或已经析构完毕，是否修改Poller里面注册的事件显得无关紧要。
    timerfdChannel_->disableAll();

    ::close(timerfd_);
    // detele timer channel
    timerfdChannel_.reset();
}

TimerId TimerQueue::addTimer(const TimerCallback& cb, const Timestamp& when, double interval)
{
    unique_ptr<Timer> timer {new Timer(cb, when, interval)};
    assert(loop_.lock());
    loop_.lock()->assertInLoopThread();
    insert(move(timer));
    return TimerId(timer.get());
}

void TimerQueue::handleRead()
{
    assert(loop_.lock());
    loop_.lock()->assertInLoopThread();
    Timestamp now(Timestamp::now());
    showTimerfd(timerfd_, now);
    // get expierd timers
    auto expiredTimer = getExpired(now);
    for(auto& one:expiredTimer){
        one.second->run();
    }
    reset(expiredTimer, now);
}

vector<pair<Timestamp, unique_ptr<Timer>>> TimerQueue::getExpired(Timestamp now)
{
    vector<pair<Timestamp, unique_ptr<Timer>>> expired;
    // get iter point to last expired timer+1 as end iter
    auto iter = timers_.upper_bound(now);
    std::move(timers_.begin(), iter, std::back_inserter(expired));
    // erase all expired timers
    timers_.erase(timers_.begin(), iter);
    return expired;
}

void TimerQueue::reset(std::vector<pair<Timestamp, unique_ptr<Timer>>>& expired, Timestamp when)
{
    Timestamp nextExpireTime;
    for(auto iter = expired.begin(); iter!=expired.end(); ++iter){
        if(iter->second->repeat()){
            iter->second->restart(when);
            insert(move(iter->second));
        }
    }
    if(!timers_.empty()){
        nextExpireTime = timers_.begin()->second->expiration();
        if (nextExpireTime.valid()) {
            resetTimerfd(timerfd_, nextExpireTime);
        }
    }  
}

bool TimerQueue::insert(unique_ptr<Timer> timer)
{
    auto needResetTimer {false};
    auto when = timer->expiration();
    // new timer is a earlist timer
    auto iter = timers_.begin();
    if(iter==timers_.end()||when<timers_.begin()->first){
        needResetTimer = true;
    }
    auto result = timers_.emplace(when, move(timer));

    if(needResetTimer){
        resetTimerfd(timerfd_, when);
    }
    
    return needResetTimer;
}

void TimerQueue::checkTimer()const{
    cout<<"timers_.size = "<<timers_.size()<<endl;
    for(auto&p : timers_){
        cout<<p.first.toString()<<" - "<<bool(p.second)<<endl;
    }
}
