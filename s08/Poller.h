
#ifndef _POLLER_H_
#define _POLLER_H_

#include "EventLoop.h"
#include "noncopyable.h"
#include <vector>
#include <map>
#include <memory>
#include <stdint.h>
class Channel;
class Timestamp;
// class ChannelList;
class pollfd;
// ��IO multiplexing�ķ�װ
// Poller��ӵ��Channel������Ҫ����Ȩ��ʹ��weak_ptr
class Poller {
    NONCOPYABLE(Poller)
    using ChannelList = std::vector<std::weak_ptr<Channel>>;
public:
    Poller(std::weak_ptr<EventLoop>);
    ~Poller();
    void poll(int timeout_ms, ChannelList& activeChannels);
    // need to update, channel exists, so use shared_ptr
    void updateChannel(std::shared_ptr<Channel> channel);
    // remove target channel
    void removeChannel(std::shared_ptr<Channel> channel);

    size_t pollfdCnt(){return pollfds_.size();}
private:
    void fillActiveChannels(int numEvents, ChannelList& activeChannels)const;
    using PollFdList = std::vector<pollfd> ;
    using ChannelMap = std::map<int, std::weak_ptr<Channel>>;
    std::weak_ptr<EventLoop> ownerLoop_;
    PollFdList pollfds_;
    ChannelMap channelmap_;
};

#endif