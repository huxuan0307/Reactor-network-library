#ifndef _TIMERQUEUE_H_
#define _TIMERQUEUE_H_
#include <functional>
#include <memory>
#include <set>
#include <map>
#include <vector>
#include "Callback.h"
#include "network_global.h"

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
    // thread safe function
    void addTimerInLoop(shared_ptr<Timer> timer);
    
    void cancel(TimerId timerid);
    void cancelInLoop(TimerId timerid);

    using TimerList = multimap<Timestamp, shared_ptr<Timer>> ;
    using ActiveTimerSet = std::set<pair<shared_ptr<Timer>, int64_t>>;
    // called when timer alarms
    void handleRead();

    // get all expired timers 
    vector<pair<Timestamp, shared_ptr<Timer>>> getExpired(Timestamp now);

    void reset(vector<pair<Timestamp, shared_ptr<Timer>>>& expired, Timestamp now);

    bool insert(shared_ptr<Timer> timer);
    // for debug
    void checkTimer()const;
private:
    weak_ptr<EventLoop> loop_;
    const int timerfd_;
    shared_ptr<Channel> timerfdChannel_;
    // 
    TimerList timers_;
    // for cancel()
    bool callingExpiredTimers_;
    ActiveTimerSet activeTimers_;
    ActiveTimerSet cancalingTimers_;
};
#endif