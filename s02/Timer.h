

#include <functional>
#include "Callback.h"
#include "Timestamp.h"
#include "noncopyable.h"
#include <iostream>

using std::cout;
using std::endl;
using std::function;

class Timer {
    NONCOPYABLE(Timer)
public:
    Timer(const TimerCallback& cb, Timestamp expiration, double interval)
        : callback_{cb},
          expiration_{expiration},
          interval_{interval},
          repeat_{interval > 0.0} {}
    void run() const { callback_(); }
    Timestamp expiration() const { return expiration_; }
    bool repeat() const { return repeat_; }
    void restart(Timestamp when);
private:
    TimerCallback callback_;
    Timestamp expiration_;
    const double interval_;
    const bool repeat_;
};