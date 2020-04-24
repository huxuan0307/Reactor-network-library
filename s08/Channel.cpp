
#include "Channel.h"

#include <poll.h>

#include <cassert>
#include <iostream>

#include "Timestamp.h"
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
    assert(!eventHandling_);    // assert if handling event while destructing
    /// @bug ? the proper time when remove Channel from Poller
    // remove channel from poller
    // if (loop_.lock())
    //     loop_.lock()->removeChannel(shared_from_this());
}

void Channel::update(){
    // cerr<<"Channel::update()"<<endl;
    if (loop_.lock())
        loop_.lock()->updateChannel(shared_from_this());
    // cerr<<"Channel::update() end"<<endl;
}

void Channel::handleEvent(Timestamp){
    eventHandling_ = true;
    if(revents_ & POLLNVAL){
        std::cerr<<"Channel::handleEvent() POLLNVAL"<<endl;
    }

    if ((revents_ & POLLHUP) && !(revents_ & POLLIN)) {
        std::cerr << "Channel::handleEvent() POLLHUP" << endl;
        if (closeCallback_) closeCallback_();
    }

    if (revents_ & (POLLERR | POLLNVAL)) {
        if (errorCallback_) errorCallback_();
    }
    if (revents_ & POLLIN) {
        if (readCallback_) readCallback_(Timestamp::now());
    }
    if (revents_ & POLLOUT) {
        if (writeCallback_) writeCallback_();
    }
    eventHandling_ = false;
}
