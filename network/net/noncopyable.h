
#define NONCOPYABLE(x) \
public:\
    x(const x&) = delete;\
    void operator=(const x&) = delete;