
#ifndef _TIMERID_H_
#define _TIEMRID_H_

#include <memory>
using std::shared_ptr;
using std::weak_ptr;
class Timer;

class TimerId {
public:
    TimerId() : timer_{}, sequence_{0} {}
    TimerId(Timer* timer, int64_t seq=0)
        : timer_{timer}, sequence_{seq} {}

private:
    Timer* timer_;
    int64_t sequence_;
    friend class TimerQueue;
};

#endif