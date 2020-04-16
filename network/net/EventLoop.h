
#include "noncopyable.h"
#include <functional>
#include <thread>

using namespace std::this_thread;
// class channel;
// class Poller;


class EventLoop{
    NONCOPYABLE(EventLoop)
public:
using event_t = std::function<void()>;

    EventLoop();
    ~EventLoop();
    void loop();
    void assertInLoopThread(){
        if(!isInLoopThread()){
            abortNotInLoopThread();
        }
    }
    bool isInLoopThread() const{return threadId_ == std::this_thread::get_id();}
    EventLoop* getEventLoopOfCurrentThread();
private:
    void abortNotInLoopThread();
    bool looping_;
    bool quit_;
    const std::thread::id threadId_;

};