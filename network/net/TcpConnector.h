
#include <functional>
#include <memory>

#include "EventLoop.h"
#include "InetAddress.h"
#include "TimerId.h"
#include "network_global.h"
class TcpConnector
{
    NONCOPYABLE(TcpConnector)
private:
    enum class State{ disconnected, connecting, connected };
    using NewConnectionCallback_t = std::function<void(int sockfd)>;
public:
    TcpConnector(std::weak_ptr<EventLoop> loop, const InetAddress& serverAddr);
    ~TcpConnector();
    void setRetryDelay(size_t ms) { retryDelay_ms_ = ms; }
    void setNewConnectionCallback(const NewConnectionCallback_t& cb);
    void setState(State s) { state_ = s; }

    void start();   // 
    void restart(); // must be called in loop thread
    void stop();
    void retry(int sockfd);

private:
    void startInLoop();
    void connect();
    void handleRead();
    void handleWrite();
    void handleError();
    void handleClose();
    void connecting(int sockfd);
    void resetChannel();
    void removeChannel();
    int removeAndResetChannel();
private:
    static const size_t kInitRetryDelay_ms = 500;
    static const size_t kMaxRetryDelay_ms = 30 * 1000;

    std::weak_ptr<EventLoop> loop_;
    InetAddress serverAddr_;
    bool connect_;
    State state_;
    size_t retryDelay_ms_;
    int sockfd_;
    TimerId timerid_;
    std::shared_ptr<Channel> channel_;
    NewConnectionCallback_t newConnectionCallback_;
};