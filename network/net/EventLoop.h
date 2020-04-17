
#include "noncopyable.h"
#include <functional>
#include <thread>
#include <vector>
#include <memory>
using namespace std::this_thread;
class Channel;
class Poller;


class EventLoop{
    NONCOPYABLE(EventLoop)
public:
using event_t = std::function<void()>;

    EventLoop();
    ~EventLoop();
    void loop();
    void quit();
    void assertInLoopThread(){
        if(!isInLoopThread()){
            abortNotInLoopThread();
        }
    }
    bool isInLoopThread() const{return threadId_ == std::this_thread::get_id();}
    EventLoop* getEventLoopOfCurrentThread();
    void updateChannel(std::weak_ptr<Channel>);

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