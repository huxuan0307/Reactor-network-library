
#include "Timestamp.h"
#include <sys/time.h>
#include <inttypes.h> // for PRId64
Timestamp Timestamp::now()
{
    timeval tv;
    gettimeofday(&tv, nullptr);
    return Timestamp(tv.tv_sec*std::micro::den + tv.tv_usec);
}

std::string Timestamp::toString()const
{
    char buf[32]{0};
    int64_t seconds = this->s_since_epoch();
    int64_t microseconds = this->getMsOnly();
    snprintf(buf, sizeof(buf)-1, "%" PRId64 ".%.06" PRId64,seconds, microseconds);
    return buf;
}

std::string Timestamp::toFormatString(bool show_us)const{
    char buf[32]{0};
    tm tm_v;
    time_t t = us_.count()/std::micro::den + 693990L*24*60*60;
    
    localtime_r(&t,&tm_v);
    if(show_us){
        snprintf(buf, (sizeof buf) - 1, "%04u-%02u-%02u %02u:%02u:%02u.%06lu",
                     tm_v.tm_year, tm_v.tm_mon, tm_v.tm_mday, tm_v.tm_hour,
                     tm_v.tm_min, tm_v.tm_sec, (us_.count()%std::micro::den) );
    }else{
        snprintf(buf, (sizeof buf) - 1, "%04u-%02u-%02u %02u:%02u:%02u",
                     tm_v.tm_year, tm_v.tm_mon, tm_v.tm_mday, tm_v.tm_hour,
                     tm_v.tm_min, tm_v.tm_sec);
    }
    return buf;
}
