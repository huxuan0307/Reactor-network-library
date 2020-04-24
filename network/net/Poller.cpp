
#include "Poller.h"
#include <poll.h>
#include "Channel.h"
#include <iostream>
#include <cassert>

using std::cerr;
using std::endl;
Poller::Poller(std::weak_ptr<EventLoop> loop)
:ownerLoop_{loop}{

}
Poller::~Poller(){

}

void Poller::poll(int timeout_ms, ChannelList& activeChannels)
{
    TRACE<<"Poller::poll()";
    int numEvents = ::poll(&pollfds_.front(), pollfds_.size(), timeout_ms);
    if(numEvents > 0){
        fillActiveChannels(numEvents, activeChannels);
    }else if(numEvents == 0){

    }else{
        perror("poll");
    }

}

void Poller::fillActiveChannels(int numEvents, ChannelList& activeChannels)const
{
    TRACE<<"Poller::fillActiveChannels()";
    
    for(auto iter = pollfds_.begin(); iter!=pollfds_.end()&&numEvents>0 ;++iter ){
        if(iter->revents>0){
            --numEvents;
            auto ch = channelmap_.find(iter->fd);
            auto channel = ch->second;
            channel.lock()->setRevents(iter->revents);
            activeChannels.push_back(channel);
        }
    }
}

void Poller::updateChannel(std::shared_ptr<Channel> channel){
    TRACE<<"Poller::updateChannel()";

    if(channel->index()<0){
        // a new one, needed to add to pollfds_
        pollfd pfd;
        pfd.fd = channel->fd();
        pfd.events = static_cast<short>(channel->events());
        
        pfd.revents = 0;
        pollfds_.push_back(pfd);
        
        int idx = static_cast<int>(pollfds_.size())-1;
        channel->setIndex(idx);

        TRACE << "new pollfd index:" << idx;
        channelmap_.emplace(pfd.fd, channel);
        
    }else{
        // a old one
        
        int idx = channel->index();
        pollfds_[idx].events = static_cast<short>(channel->events());
        pollfds_[idx].revents = 0;
        if(channel->isNoneEvent()){
            // ignore this fd
            pollfds_[idx].fd = -idx - 1;
        }

    }
}

void Poller::removeChannel(std::shared_ptr<Channel> channel)
{
    TRACE<<"Poller::removeChannel()";
    int idx = channel->index();
    assert(idx>=0);
    assert(channelmap_.find(channel->fd()) != channelmap_.end());

    this->channelmap_.erase(channel->fd());
    if(static_cast<size_t>(idx) == pollfds_.size() -1 ){
        pollfds_.pop_back();
    }else if(static_cast<size_t>(idx)<pollfds_.size() -1){
        std::iter_swap(pollfds_.begin() + idx ,pollfds_.end()-1);
        int channelfd_atend = pollfds_.back().fd;
        // update channel.index
        if(channelfd_atend<0){
            channelfd_atend = -channelfd_atend -1;
        }
        channelmap_[channelfd_atend].lock()->setIndex(idx);
        pollfds_.pop_back();
    }
}