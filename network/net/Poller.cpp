
#include "Poller.h"
#include <poll.h>
#include "Channel.h"
Poller::Poller(EventLoop* loop)
:ownerLoop_{loop}{

}
Poller::~Poller(){

}

void Poller::poll(int timeout_ms, ChannelList& activeChannels)
{
    int numEvents = ::poll(&pollfds_.front(), pollfds_.size(), timeout_ms);
    if(numEvents > 0){
        fillActiveChannels(numEvents, activeChannels);
    }else if(numEvents == 0){

    }else{

    }

}

void Poller::fillActiveChannels(int numEvents, ChannelList& activeChannels)const
{
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
    if(channel->index()<0){
        // a new one, needed to add to pollfds_
        pollfd pfd;
        pfd.fd = channel->fd();
        pfd.events = static_cast<short>(channel->events());
        pfd.revents = 0;
        pollfds_.push_back(pfd);
        int idx = static_cast<int>(pollfds_.size())-1;
        channel->setIndex(idx);
        channelmap_.emplace(pfd.fd, channel);
    }else{
        // a old one
        
        int idx = channel->index();
        pollfds_[idx].events = static_cast<short>(channel->events());
        pollfds_[idx].revents = 0;
        if(channel->isNoneEvent()){
            // ignore this fd
            pollfds_[idx].fd=-1;
        }

    }
}