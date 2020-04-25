
#include "network_global.h"
#include <functional>
#include <memory>
#include "EventLoop.h"
#include "InetAddress.h"
#include "TimerId.h"
class Connector
{
    NONCOPYABLE(Connector)
public:
    using NewConnectionCallback_t = std::function<void(int sockfd)>;

public:
    Connector(std::weak_ptr<EventLoop> loop, const InetAddress& serverAddr);
    ~Connector();
    void setRetryDelay(size_t ms) { retryDelay_ms_ = ms; }
    void setNewConnectionCallback(const NewConnectionCallback_t& cb);
    void setState(State s) { state_ = s; }

    void start();   // 
    void restart(); // must be called in loop thread
    void stop();

private:
    static const size_t kInitRetryDelay_ms;
    enum class State{ disconnected, connecting, connected };

    std::weak_ptr<EventLoop> loop_;
    InetAddress serverAddr_;
    bool connect_;
    State state_;
    size_t retryDelay_ms_;
    int sockfd_;
    TimerId timerid_;
    std::shared_ptr<Channel> channel_;
    NewConnectionCallback_t connectionCallback_;

private:
    void startInLoop();
    void connect();
    void handleRead();
    void handleWrite();
    void handleError();
    void handleClose();
    void Connecting(int sockfd);
};