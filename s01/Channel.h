#ifndef _CHANNEL_H_
#define _CHANNEL_H_

#include <functional>
#include <memory>

#include "noncopyable.h"
#include "EventLoop.h"
// one channel per thread?
using std::enable_shared_from_this;
class Channel:public enable_shared_from_this<Channel>
{
    NONCOPYABLE(Channel)

public:
using EventCallback = std::function<void()>;
    Channel(std::weak_ptr<EventLoop> loop, int fd);
    void handleEvent();
    void setReadCallback(const EventCallback& cb){readCallback_ = cb;}
    void setWriteCallback(const EventCallback& cb){writeCallback_ = cb;}
    void setErrorCallback(const EventCallback& cb){errorCallback_ = cb;}

    int fd()const {return fd_;}
    int events()const {return events_;}
    void setRevents(int revents){revents_ = revents;}
    bool isNoneEvent()const{return events_ == kNoneEvent;}
    void enableReading() {events_ |= kReadEvent; update();}
    void enableWriting() {events_ |= kWriteEvent; update();}
    void disableReading() {events_ &= ~kReadEvent; update();}
    void disableWriting() {events_ &= ~kWriteEvent; update();}
    void disableAll(){events_ = kNoneEvent; update();}

    // for Poller
    int index()const {return index_;}
    void setIndex(int idx){index_ = idx;}

    // for EventLoop
    std::weak_ptr<EventLoop> ownerLoop()const {return loop_;}

private:
    void update();

    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;

    std::weak_ptr<EventLoop> loop_;
    const int fd_;
    int events_;
    int revents_;
    int index_;

    EventCallback readCallback_;
    EventCallback writeCallback_;
    EventCallback errorCallback_;

};

#endif