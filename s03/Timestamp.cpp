
#include "Timestamp.h"
#include <sys/time.h>
#include <inttypes.h> // for PRId64
Timestamp Timestamp::now()
{
    timeval tv;
    gettimeofday(&tv, nullptr);
    return Timestamp(tv.tv_sec*1000000 + tv.tv_usec);
}

std::string Timestamp::toString()const
{
    char buf[32]{0};
    int64_t seconds = this->s_since_epoch();
    int64_t microseconds = this->getMsOnly();
    snprintf(buf, sizeof(buf)-1, "%" PRId64 ".%.06" PRId64,seconds, microseconds);
    return buf;
}

