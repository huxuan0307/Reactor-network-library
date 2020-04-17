
#include "Channel.h"
#include <poll.h>
#include <iostream>
const int Channel::kNoneEvent=0;
const int Channel::kReadEvent=POLLIN | POLLPRI;
const int Channel::kWriteEvent=POLLOUT;
using std::cerr;
using std::endl;
Channel::Channel(std::weak_ptr<EventLoop> loop, int fd)
    : loop_{loop}, fd_{fd}, events_{kNoneEvent}, revents_{kNoneEvent}, index_{-1} {}

void Channel::update(){
    cerr<<"Channel::update()"<<endl;
    loop_.lock()->updateChannel(shared_from_this());
}

void Channel::handleEvent(){
    if(revents_ & POLLNVAL){
        std::cerr<<"Channel::handleEvent() POLLNVAL"<<endl;
    }
    if(revents_ & (POLLERR | POLLNVAL)){
        if(errorCallback_)
            errorCallback_();
    }
    if(revents_ & POLLIN){
        if (readCallback_) readCallback_();
    }
    if(revents_ & POLLOUT){
        if(writeCallback_) writeCallback_();
    }
}
