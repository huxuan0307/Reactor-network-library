
#include "network_global.h"
#include <memory>
#include <vector>
#include "EventLoop.h"
#include "EventLoopThread.h"
class EventLoopThreadPool
{
private:
    std::weak_ptr<EventLoop> baseloop_; // main loop
    size_t threadNum_;
    bool started_;
    std::vector<std::shared_ptr<EventLoopThread>> eventLoopThreads_;
    std::vector<std::weak_ptr<EventLoop>> loops_; // extraLoops
    size_t nextThreadIndex_;
public:
    EventLoopThreadPool(std::weak_ptr<EventLoop>loop, size_t threadNum = 0);
    ~EventLoopThreadPool();
    void start();
    void startWithThreadNum(size_t threadNum) {
        threadNum_ = threadNum;
        start();
    }
    std::weak_ptr<EventLoop> getNextLoop();
    
    void setThreadNum(size_t threadNum) { threadNum_ = threadNum; }
private:
    size_t getNextLoopIndex(){
        assert(threadNum_ != 0);
        if (++nextThreadIndex_ == threadNum_) 
            nextThreadIndex_ = 0;
        return nextThreadIndex_;
    }
};