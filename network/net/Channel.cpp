
#include "Channel.h"

#include <poll.h>

#include <cassert>
#include <iostream>

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = POLLIN | POLLPRI;
const int Channel::kWriteEvent = POLLOUT;
using std::cerr;
using std::endl;
Channel::Channel(std::weak_ptr<EventLoop> loop, int fd)
    : loop_{loop},
      fd_{fd},
      events_{kNoneEvent},
      revents_{kNoneEvent},
      index_{-1},
      eventHandling_{false} {}

Channel::~Channel()
{
    Debug()<<"~Channel";
    assert(!eventHandling_);    // assert if handling event while destructing
    /// @bug ? the proper time when remove Channel from Poller
    // remove channel from poller
    // if (loop_.lock())
    //     loop_.lock()->removeChannel(shared_from_this());
}

void Channel::update(){
    TRACE<<"Channel::update()";
    if (loop_.lock())
        loop_.lock()->updateChannel(shared_from_this());
    TRACE<<"Channel::update() end";
}

void Channel::handleEvent(Timestamp){
    eventHandling_ = true;
    if(revents_ & POLLNVAL){
        CRITICAL<<"Channel::handleEvent() POLLNVAL";
    }

    if ((revents_ & POLLHUP) && !(revents_ & POLLIN)) {
        CRITICAL << "Channel::handleEvent() POLLHUP";
        if (closeCallback_) closeCallback_();
    }

    if (revents_ & (POLLERR | POLLNVAL)) {
        TRACE << "Channel::handleEvent() (POLLERR | POLLNVAL)";
        if (errorCallback_) errorCallback_();
    }
    if (revents_ & POLLIN) {
        TRACE << "Channel::handleEvent() POLLIN";
        if (readCallback_) readCallback_(Timestamp::now());
    }
    if (revents_ & POLLOUT) {
        TRACE << "Channel::handleEvent() POLLOUT";
        if (writeCallback_) writeCallback_();
    }
    eventHandling_ = false;
}
