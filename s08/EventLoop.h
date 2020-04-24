
#ifndef _EVENTLOOP_H_
#define _EVENTLOOP_H_
#include "noncopyable.h"
#include "Callback.h"
#include <functional>
#include <thread>
#include <vector>
#include <memory>
#include <mutex>
using namespace std::this_thread;
using std::enable_shared_from_this;

class Channel;
class Poller;
class TimerId;
class Timestamp;
class TimerQueue;
class EventLoop:public enable_shared_from_this<EventLoop>
{
    NONCOPYABLE(EventLoop)
public:

    EventLoop();
    ~EventLoop();
    void init();

    void loop();
    void quit();
    void assertInLoopThread(){
        if(!isInLoopThread()){
            abortNotInLoopThread();
        }
    }
    bool isInLoopThread() const{return threadId_ == std::this_thread::get_id();}
    EventLoop* getEventLoopOfCurrentThread();
    void updateChannel(std::shared_ptr<Channel>);
    void removeChannel(std::shared_ptr<Channel>);
    TimerId run_at(const Timestamp& timestamp, const TimerCallback& cb);
    TimerId run_after(double delay, const TimerCallback& cb);
    TimerId run_every(double interval, const TimerCallback& cb);

    void runInLoop(const Event& cb);
    void queueInLoop(const Event& cb);
    void wakeup();
    size_t pollfdCnt();
private:
    using ChannelList = std::vector<std::weak_ptr<Channel>>;

    void abortNotInLoopThread();
    void handleRead();      // wake up?
    void doPendingFunctors();

    bool looping_;
    bool quit_;
    bool callingPendingFunctors_;   // atomic?
    const std::thread::id threadId_;
    std::unique_ptr<Poller> poller_;
    ChannelList activeChannels_;
    std::unique_ptr<TimerQueue> timerQueue_;
    static const int kPOllerTime_ms;
    int wakeupFd_;
    std::shared_ptr<Channel> wakeupChannel_;
    std::vector<Event> pendingFunctors_;
    mutable std::mutex mutex_;
};

#endif