#ifndef _ACCEPTOR_H_
#define _ACCEPTOR_H_
#include <functional>
#include "network_global.h"

#include "Callback.h"
#include "Socket.h"
#include "Channel.h"
#include <memory>
using std::weak_ptr;
using std::shared_ptr;

class EventLoop;
class InetAddress;
class Channel;
class Socket;
// accept new connection and notify caller 
// 
class Acceptor
{
NONCOPYABLE(Acceptor)
public:

    Acceptor(weak_ptr<EventLoop> loop, const InetAddress& listenAddr);
    ~Acceptor(){};

    void setNewConnecttionCallback(const NewConnectionCallback_t& cb) {
        newConnectionCallback_ = cb;
    }
    bool is_listenning() const { return listenning_; }
    void listen();
private:
    void handleRead();
    weak_ptr<EventLoop> loop_;
    Socket acceptSocket_;
    shared_ptr<Channel> acceptChannel_;
    NewConnectionCallback_t newConnectionCallback_;
    bool listenning_;
};

#endif