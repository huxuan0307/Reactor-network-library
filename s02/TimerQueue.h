#ifndef _TIMERQUEUE_H_
#define _TIMERQUEUE_H_
#include <functional>
#include <memory>
#include <set>
#include <map>
#include <vector>
#include "Callback.h"
// #include "Channel.h"
// #include "EventLoop.h"
// #include "TimerId.h"
#include "Timestamp.h"
#include "noncopyable.h"
using std::shared_ptr;
using std::unique_ptr;
using std::weak_ptr;
using std::pair;
using std::vector;
using std::multimap;
class EventLoop;
class Channel;
class TimerId;
class Timer;
class TimerQueue {
    NONCOPYABLE(TimerQueue)
public:
    TimerQueue(weak_ptr<EventLoop> loop);
    ~TimerQueue();
    TimerId addTimer(const TimerCallback& cb, const Timestamp& when, double interval);
    void cancel(TimerId timerid);
    
    using TimerList = multimap<Timestamp, unique_ptr<Timer>> ;
    // called when timer alarms
    void handleRead();

    // get all expired timers 
    vector<pair<Timestamp, unique_ptr<Timer>>> getExpired(Timestamp now);

    void reset(vector<pair<Timestamp, unique_ptr<Timer>>>& expired, Timestamp now);

    bool insert(unique_ptr<Timer> timer);
    // for debug
    void checkTimer()const;
private:
    weak_ptr<EventLoop> loop_;
    const int timerfd_;
    shared_ptr<Channel> timerfdChannel_;
    // 
    TimerList timers_;
};
#endif