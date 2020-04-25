
#include <condition_variable>
#include <mutex>
#include <thread>

#include "../base/noncopyable.h"
using std::shared_ptr;
using std::unique_ptr;
class EventLoop;
class EventLoopThread {
    NONCOPYABLE(EventLoopThread)
public:
    EventLoopThread();
    ~EventLoopThread();
    shared_ptr<EventLoop> startLoop();

private:
    void threadFunc();
    
    shared_ptr<EventLoop> loop_;
    bool exiting_;
    unique_ptr<std::thread> thread_;
    std::mutex mutex_;
    std::condition_variable cond_;
};