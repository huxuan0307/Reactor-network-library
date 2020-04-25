
#include "TcpServer.h"
#include "InetAddress.h"
#include "TcpConnection.h"
#include <iostream>
#include <cassert>
using std::cout;
using std::cerr;
using std::endl;
namespace placeholders = std::placeholders;

TcpServer::TcpServer(weak_ptr<EventLoop> loop, const InetAddress& listenAddr)
:loop_{loop}, acceptor_{new Acceptor{loop, listenAddr}},
started_{false},nextConnId_{1},name_{listenAddr.toIpPort()},
threadPool_{loop}
{
    assert(loop.lock());
    acceptor_->setNewConnecttionCallback([this](int sockfd, const InetAddress& peerAddr){
        this->newConnection(sockfd, peerAddr);
    });
}

TcpServer::~TcpServer(){}

void TcpServer::start()
{
    TRACE << "TcpServer::start()";
    assert(loop_.lock());
    loop_.lock()->init();
    
    assert(!started_);
    started_ = true;
    threadPool_.start();
    
    if(!acceptor_->is_listenning()){
        loop_.lock()->runInLoop([this](){
            acceptor_->listen();
        });
    }
}

void TcpServer::newConnection(int sockfd, const InetAddress& peerAddr){
    Debug()<<"new connection comes from"<<peerAddr.toIpPort();
    TRACE << "TcpServer::newConnection()";
    assert(loop_.lock());
    loop_.lock()->assertInLoopThread();
    string connName = name_ + std::to_string(getNextConnId());
    cout << "TcpServer::newConnection() [" << name_ << "] - new connection ["
         << connName << "] from " << peerAddr.toIpPort() << endl;
    InetAddress localAddr(sockets::getLocalAddr(sockfd));
    auto ioloop = threadPool_.getNextLoop();
    shared_ptr<TcpConnection> conn{new TcpConnection{ioloop, connName, sockfd, localAddr, peerAddr}};
    connectionMap_[connName] = conn;
    conn->setConnectionCallback(connectionCallback_);
    conn->setMessageCallback(messageCallback_);
    conn->setWriteCompleteCallback(writeCompleteCallback_);
    conn->setCloseCallback([this](const shared_ptr<TcpConnection>& conn) {
        removeConnection(conn);
    });
    ioloop.lock()->runInLoop(std::bind(&TcpConnection::connectEstablished, conn));

    TRACE << "TcpServer::newConnection() done!";
}


void TcpServer::removeConnection(const shared_ptr<TcpConnection>& conn)
{
    Debug()<<"connection" << conn->name() << " is down";

    TRACE << "TcpServer::removeConnection()";
    loop_.lock()->runInLoop(std::bind(&TcpServer::removeConnectionInLoop, this, conn));
    TRACE << "TcpServer::removeConnection() done!";
}

void TcpServer::removeConnectionInLoop(const shared_ptr<TcpConnection>& conn){
    TRACE <<"TcpServer::removeConnectionInLoop()";
    assert(loop_.lock());
    loop_.lock()->assertInLoopThread();
    cout << "TcpServer::removeConnection() [" << name_ << "] - connection ["
         << conn->name() << endl;
    auto ioloop = conn->getLoop();
    assert(connectionMap_.erase(conn->name())==1);
    ioloop.lock()->queueInLoop([conn](){
        conn->connectDestroyed();
    });
    TRACE << "TcpServer::removeConnectionInLoop() done!";
}