
#ifndef _TCPCONNECTION_H_
#define _TCPCONNECTION_H_

#include "Callback.h"
#include "noncopyable.h"
#include "InetAddress.h"
#include <memory>
using std::unique_ptr;
using std::shared_ptr;
using std::weak_ptr;
using std::string;

class Socket;
class EventLoop;
class Channel;

class TcpConnection:public std::enable_shared_from_this<TcpConnection>
{
NONCOPYABLE(TcpConnection)
public:
    TcpConnection(weak_ptr<EventLoop> loop, const string& connName, int sockfd,
                  const InetAddress& localAddr, const InetAddress& peerAddr);
    ~TcpConnection();
    void setConnectionCallback(const ConnectionCallback_t& cb){
        connectionCallback_ = cb;
    }
    void setMessageCallback(const MessageCallback_t& cb){
        messageCallback_ = cb;
    }
    void setCloseCallback(const CloseCallback_t& cb){
        closeCallback_ = cb;
    }
    // called when TcpServer accepts a new connection
    void connectEstablished();
    // called when receive 0 bytes
    void connectDestroyed();

    bool connected(){
        return State::connected == state_;
    }
    bool disconnected(){
        return State::disconnected == state_;
    }
    
    string& name(){
        return name_;
    }
    InetAddress& peerAddr(){
        return peerAddr_;
    }
    InetAddress& localAddr() { return localAddr_; }

private:
    enum class State{connecting, connected, disconnected, };
    void setState(State s){state_ = s;}
    void handleRead();
    void handleWrite();
    void handleClose();
    void handleError();
    State state_;
    weak_ptr<EventLoop> loop_;
    unique_ptr<Socket> socket_;
    shared_ptr<Channel> channel_;
    InetAddress localAddr_;
    InetAddress peerAddr_;
    string name_; // for log and debug
    ConnectionCallback_t connectionCallback_;
    MessageCallback_t messageCallback_;
    CloseCallback_t closeCallback_;
};

#endif