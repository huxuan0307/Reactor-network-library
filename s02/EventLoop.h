
#ifndef _EVENTLOOP_H_
#define _EVENTLOOP_H_
#include "noncopyable.h"
#include "Callback.h"
// #include "TimerQueue.h"
#include <functional>
#include <thread>
#include <vector>
#include <memory>
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
using event_t = std::function<void()>;

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

    std::unique_ptr<TimerQueue> timerQueue_;
private:
    using ChannelList = std::vector<std::weak_ptr<Channel>>;
    void abortNotInLoopThread();
    bool looping_;
    bool quit_;
    const std::thread::id threadId_;
    std::unique_ptr<Poller> poller_;
    ChannelList activeChannels_;
    static const int kPOllerTime_ms;
};

#endif