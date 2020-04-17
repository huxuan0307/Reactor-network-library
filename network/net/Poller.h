
#include "EventLoop.h"
#include "noncopyable.h"
#include <vector>
#include <map>
#include <memory>
class Channel;
class TimeStamp;
// class ChannelList;
class pollfd;
// 对IO multiplexing的封装
// Poller不拥有Channel，不需要管理权，使用weak_ptr
class Poller {
    NONCOPYABLE(Poller)
    using ChannelList = std::vector<std::weak_ptr<Channel>>;
public:
    Poller(EventLoop*);
    ~Poller();
    void poll(int timeout_ms, ChannelList& activeChannels);
    // need to update, channel exists, so use shared_ptr
    void Poller::updateChannel(std::shared_ptr<Channel> channel);

private:
    void fillActiveChannels(int numEvents, ChannelList& activeChannels)const;
    using PollFdList = std::vector<pollfd> ;
    using ChannelMap = std::map<int, std::weak_ptr<Channel>>;
    EventLoop* ownerLoop_;
    PollFdList pollfds_;
    ChannelMap channelmap_;
};