
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
    chrono::microseconds ms_;

public:
    explicit Timestamp(int64_t ms=0) : ms_{ms} {}
    Timestamp(const Timestamp& right){
        ms_ = right.ms_;
    }
    Timestamp& operator=(const Timestamp& right){
        ms_ = right.ms_;
        return *this;
    }
    ~Timestamp() {}
    bool valid() { return ms_.count() > 0; }
    void swap(Timestamp& right) { std::swap(ms_, right.ms_); }
    string toString() const;
    string toFormatString(bool show_ms = true) const;
    int64_t ms_since_epoch() const { return ms_.count(); }
    time_t s_since_epoch() const { return chrono::duration_cast<chrono::seconds>(ms_).count(); }
    int64_t getMsOnly() const { return ms_.count() % micro::den; }
    bool operator<(const Timestamp& right)const { return ms_ < right.ms_; }
    bool operator==(const Timestamp& right)const { return ms_ == right.ms_; }
    Timestamp& operator+=(double secs) {
        ms_ += chrono::duration_cast<chrono::microseconds>(chrono::duration<double>(secs));
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
    return chrono::duration_cast<chrono::duration<double>>(left.ms_ - right.ms_).count();
}

inline Timestamp operator-(const Timestamp& left, const Timestamp& right) {
    return Timestamp(left.ms_.count() - right.ms_.count());
}

#endif