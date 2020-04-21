
#ifndef _TCPSERVER_H_
#define _TCPSERVER_H_
#include <memory>
#include <map>
#include "noncopyable.h"
#include "EventLoop.h"
#include "Callback.h"
#include "Acceptor.h"
#include <string>
using std::unique_ptr;
using std::shared_ptr;
using std::weak_ptr;
using std::string;
class TcpServer
{
NONCOPYABLE(TcpServer)
public:
    TcpServer(weak_ptr<EventLoop> loop, const InetAddress& listenAddr);
    ~TcpServer();

    void start();

    void setConnectionCallback(const ConnectionCallback_t& cb){
        connectionCallback_ = cb;
    }

    void setMessageCallback(const MessageCallback_t& cb){
        messageCallback_ = cb;
    }
private:
    void newConnection(int sockfd, const InetAddress& peerAddr);
    int getNextConnId(){
        return nextConnId_++;
    }
    using ConnectionMap_t = std::map<string, shared_ptr<TcpConnection>>;
    weak_ptr<EventLoop> loop_;
    shared_ptr<Acceptor> acceptor_;
    ConnectionCallback_t connectionCallback_;
    MessageCallback_t messageCallback_;

    string name_;
    bool started_;
    int nextConnId_;
    ConnectionMap_t connectionMap_;

};

#endif