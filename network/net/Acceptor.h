
#include <noncopyabal.h>
#include <functional>
#include "Socket.h"

class EventLoop;
class InetAddress;

class Accepter
{
NONCOPYABLE(Accepter)
public:
    using NewConnection_t =
        std::function<void(int sockfd, const InetAddress &)>;
    Accepter(EventLoop* loop, const InetAddress& listenAddr, bool reuseport);
    ~Accepter();

    void setNewConnecttionCallback(const NewConnection_t& cb){}

    private:
    void handleRead();
    EventLoop* loop_;
    Socket acceptSocket_;
    Channel acceptChannel_;
    NewConnection_t newConnectionCallback_;
    bool listenning_;
    int idlefd_;
};