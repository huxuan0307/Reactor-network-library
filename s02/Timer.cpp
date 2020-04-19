
#include "Timer.h"

void Timer::restart(Timestamp when)
{
    if(repeat_){
        expiration_ = when += interval_;
    }
    else{
        expiration_ = Timestamp{};
    }
}