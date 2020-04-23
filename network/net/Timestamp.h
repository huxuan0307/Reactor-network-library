
#ifndef _TIMESTAMP_H_
#define _TIMESTAMP_H_
#include <stdint.h>

#include <chrono>
#include <cstring>
#include <string>

namespace chrono = std::chrono;

using std::micro;
using std::string;
// using std::chrono::duration;
// using std::chrono::duration_cast;
// using std::chrono::microseconds;
// using std::chrono::seconds;
class Timestamp {
private:
    chrono::microseconds us_;

public:
    explicit Timestamp(int64_t ms=0) : us_{ms} {}
    Timestamp(const Timestamp& right){
        us_ = right.us_;
    }
    Timestamp& operator=(const Timestamp& right){
        us_ = right.us_;
        return *this;
    }
    ~Timestamp() {}
    bool valid() { return us_.count() > 0; }
    void swap(Timestamp& right) { std::swap(us_, right.us_); }
    string toString() const;
    string toFormatString(bool show_us = true) const;
    int64_t ms_since_epoch() const { return us_.count(); }
    time_t s_since_epoch() const { return chrono::duration_cast<chrono::seconds>(us_).count(); }
    int64_t getMsOnly() const { return us_.count() % micro::den; }
    bool operator<(const Timestamp& right)const { return us_ < right.us_; }
    bool operator==(const Timestamp& right)const { return us_ == right.us_; }
    Timestamp& operator+=(double secs) {
        us_ += chrono::duration_cast<chrono::microseconds>(chrono::duration<double>(secs));
        return *this;
    }

    static Timestamp now();

    friend double timeDiff(const Timestamp& left, const Timestamp& right);
    friend Timestamp operator-(const Timestamp& left, const Timestamp& right);
};

///
/// @return (a-b) in seconds
//
//
inline double timeDiff(const Timestamp& left, const Timestamp& right) {
    return chrono::duration_cast<chrono::duration<double>>(left.us_ - right.us_).count();
}

inline Timestamp operator-(const Timestamp& left, const Timestamp& right) {
    return Timestamp(left.us_.count() - right.us_.count());
}

#endif