
#ifndef _TCPSERVER_H_
#define _TCPSERVER_H_

#include <map>
#include <memory>
#include <string>

#include "Acceptor.h"
#include "Callback.h"
#include "EventLoop.h"
#include "network_global.h"
#include "EventLoopThreadPool.h"
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

    void setWriteCompleteCallback(const WriteCompleteCallback_t& cb){
        writeCompleteCallback_ =  cb;
    }

    void setThreadNum(size_t threadNum){
        threadPool_.setThreadNum(threadNum);
    }

    void startWithThreadNum(size_t threadNum){
        threadPool_.setThreadNum(threadNum);
        start();
    }

private:
    void newConnection(int sockfd, const InetAddress& peerAddr);
    void removeConnection(const shared_ptr<TcpConnection>& conn);
    void removeConnectionInLoop(const shared_ptr<TcpConnection>& conn);
    int getNextConnId(){
        return nextConnId_++;
    }
    using ConnectionMap_t = std::map<string, shared_ptr<TcpConnection>>;
    weak_ptr<EventLoop> loop_;
    shared_ptr<Acceptor> acceptor_;

    bool started_;
    int nextConnId_;
    string name_;
    ConnectionMap_t connectionMap_;

    ConnectionCallback_t connectionCallback_;
    MessageCallback_t messageCallback_;
    WriteCompleteCallback_t writeCompleteCallback_;

    EventLoopThreadPool threadPool_;
};

#endif