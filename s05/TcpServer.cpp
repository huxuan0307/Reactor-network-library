
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
:loop_{loop}, name_{listenAddr.toIpPort()}, acceptor_{new Acceptor{loop, listenAddr}},
started_{false},nextConnId_{1}
{
    assert(loop.lock());
    acceptor_->setNewConnecttionCallback([this](int sockfd, const InetAddress& peerAddr){
        this->newConnection(sockfd, peerAddr);
    });
}

TcpServer::~TcpServer(){}

void TcpServer::start()
{
    assert(loop_.lock());
    loop_.lock()->init();
    assert(!started_);
    started_ = true;
    if(!acceptor_->is_listenning()){
        loop_.lock()->runInLoop([this](){
            acceptor_->listen();
        });
    }

}

void TcpServer::newConnection(int sockfd, const InetAddress& peerAddr){
    cout<<"TcpServer::newConnection()"<<endl;
    assert(loop_.lock());
    loop_.lock()->assertInLoopThread();
    string connName = name_ + std::to_string(getNextConnId());
    cout << "TcpServer::newConnection() [" << name_ << "] - new connection ["
         << connName << "] from " << peerAddr.toIpPort() << endl;
    InetAddress localAddr(sockets::getLocalAddr(sockfd));
    shared_ptr<TcpConnection> conn{new TcpConnection{loop_, connName, sockfd, localAddr, peerAddr}};
    connectionMap_[connName] = conn;
    conn->setConnectionCallback(connectionCallback_);
    conn->setMessageCallback(messageCallback_);
    conn->connectEstablished();

    cout<<"TcpServer::newConnection() end"<<endl;
}