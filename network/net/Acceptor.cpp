
#include "Acceptor.h"
#include <cassert>
Acceptor::Acceptor(weak_ptr<EventLoop> loop, const InetAddress& listenAddr)
:loop_{loop},
acceptSocket_{sockets::createNonblockingSocket(AF_INET)},
acceptChannel_{new Channel{loop, acceptSocket_.fd()}},
listenning_{false}
{
    acceptSocket_.setReuseAddr();
    acceptSocket_.bindAddress(listenAddr);
}

void Acceptor::listen()
{
    assert(loop_.lock());
    loop_.lock()->assertInLoopThread();
    listenning_ = true;
    acceptSocket_.listen();
    // put setreadback & enable togather
    acceptChannel_->setReadCallback([this](){
        handleRead();
    });
    acceptChannel_->enableReading();
}

// accept one connection per call
void Acceptor::handleRead()
{
    assert(loop_.lock());
    loop_.lock()->assertInLoopThread();
    InetAddress peerAddr(sockaddr_in{});
    int connfd = acceptSocket_.accept(&peerAddr);
    if(connfd >=0 ){
        newConnectionCallback_(connfd, peerAddr);
    }else{
        sockets::close(connfd);
    }
}





