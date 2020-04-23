
#include "TcpConnection.h"
#include <cstring>
#include <unistd.h>
#include <cassert>
#include "Channel.h"
#include "Socket.h"
#include <iostream>
#include <cassert>
#include "Timestamp.h"
using std::cout;
using std::cerr;
using std::endl;

TcpConnection::TcpConnection(weak_ptr<EventLoop> loop, const string& connName,
                             int sockfd, const InetAddress& localAddr,
                             const InetAddress& peerAddr)
    : loop_{loop},
      socket_{new Socket{sockfd}},
      channel_{new Channel{loop, sockfd}},
      localAddr_{localAddr},
      peerAddr_{peerAddr},
      name_{connName},
      state_{State::connecting} {
    cout<<"TcpConnection::TcpConnection()... "<<endl;
    channel_->setReadCallback([this](Timestamp t){
        this->handleRead(t);
    });
    channel_->setWriteCallback([this](){
        this->handleWrite();
    });
    channel_->setCloseCallback([this](){
        this->handleClose();
    });
    channel_->setErrorCallback([this](){
        this->handleError();
    });
    cout<<"TcpConnection::TcpConnection() end "<<endl;
}

TcpConnection::~TcpConnection() {
    cout<<"~TcpConnection()"<<endl;
}

void TcpConnection::handleRead(Timestamp receiveTime) {
    cout<<"TcpConnection::handleRead()"<<endl;
    int savedErrno = 0;
    // char buf[65536];
    // ssize_t cnt = ::read(channel_->fd(), buf, sizeof buf);
    ssize_t cnt = inputBuffer_.readFromFd(channel_->fd(), savedErrno);
    if(cnt>0){
        messageCallback_(shared_from_this(), &inputBuffer_, receiveTime);
    }
    else if(cnt==0){
        handleClose();
    }
    else{
        errno = savedErrno;
        perror("TcpConnection::handleRead");
        handleError();
    }
}

void TcpConnection::handleWrite()
{
    cout<<"TcpConnection::handleWrite()"<<endl;
    
}

// 连接关闭的流程
// 1. 收到peer的close，即read()=0
// 2. 调用TcpConnection::handleClose()
//   2.1 调用Channel::disableAll() 在Poller中注销此连接的全部事件
// 3. 调用定义的回调函数，通知用户连接已经关闭
// 4. 调用TcpConnection::connectDestroyed()
//   4.1 调用连接回调connectionCallback_？
// 5. 在Poller中清除当前Channel
void TcpConnection::handleClose()
{
    assert(loop_.lock());
    loop_.lock()->assertInLoopThread();
    assert(state_==State::connected);

    channel_->disableAll();
    closeCallback_(shared_from_this());
    // loop_.lock()->removeChannel(channel_);
}

void TcpConnection::handleError()
{
    int err = sockets::getSocketError(channel_->fd());
    cout << "TcpConnection::handleError() [" << name_
         << "] - SO_ERROR = " << err << " " << strerror(err) << endl;
}

void TcpConnection::connectEstablished()
{
    cout<<"TcpConnection::connectEstablished()"<<endl;
    assert(loop_.lock());
    assert(connectionCallback_&&messageCallback_&&closeCallback_);

    loop_.lock()->assertInLoopThread();
    assert(state_==State::connecting);
    setState(State::connected);
    channel_->enableReading();
    connectionCallback_(shared_from_this());
}

void TcpConnection::connectDestroyed()
{
    cout<<"TcpConnection::connectDestroyed()"<<endl;
    assert(loop_.lock());
    loop_.lock()->assertInLoopThread();
    assert(state_==State::connected);
    setState(State::disconnected);
    if (channel_->events() != Channel::kNoneEvent) 
        channel_->disableAll();
    connectionCallback_(shared_from_this()); // ? why not closeCallback
    loop_.lock()->removeChannel(channel_);
    cout<<"TcpConnection::connectDestroyed() end"<<endl;

}