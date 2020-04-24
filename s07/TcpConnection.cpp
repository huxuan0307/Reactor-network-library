
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
    assert(loop_.lock());
    loop_.lock()->assertInLoopThread();
    if(channel_->isWriting()){
        // write only once since if cannot write all data once, cannot second time.
        auto cnt = ::write(channel_->fd(), outputBuffer_.beginRead(), outputBuffer_.readableLength());
        if(cnt>0){
            outputBuffer_.retrieve(cnt);
            if(outputBuffer_.readableLength() == 0){
                // write all unset POLLOUT
                channel_->disableWriting();
                // this flag setted when TcpConnection::shutdown() called
                if(state_ == State::disconnecting){
                    shutdownInLoop();
                }
            }else{
                // more data will be written next time
            }
        }else{
            perror("write()");
            cerr<<"TcpConnection::handleWrite()"<<endl;
        }
    }else{
        // do nothing
    }

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
    assert(state_==State::connected||state_==State::disconnecting);

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
    assert(state_==State::connected||state_==State::disconnecting);
    setState(State::disconnected);
    if (channel_->events() != Channel::kNoneEvent) 
        channel_->disableAll();
    connectionCallback_(shared_from_this()); // ? why not closeCallback
    loop_.lock()->removeChannel(channel_);
    cout<<"TcpConnection::connectDestroyed() end"<<endl;

}

void TcpConnection::shutdown()
{
    cout<<"TcpConnection::shutdown()"<<endl;
    assert(loop_.lock());
    // assert(state_ == State::connected);
    if(state_ == State::connected){
        // set half close flag, close when nothing to write
        setState(State::disconnecting);
        loop_.lock()->runInLoop([this](){
            shutdownInLoop();
        });
    }
}

void TcpConnection::shutdownInLoop(){
    cout<<"TcpConnection::shutdownInLoop()"<<endl;
    assert(loop_.lock());
    loop_.lock()->assertInLoopThread();
    if(!channel_->isWriting()){
        socket_->shutdownWrite();
    }
}

void TcpConnection::send(string&& message)
{
    if(state_ == State::connected){
        if(loop_.lock()->isInLoopThread()){
            sendInLoop(move(message));
        }
        else{
            loop_.lock()->runInLoop([message, this]()mutable{
                sendInLoop(move(message));
            });
        }
    }
}

void TcpConnection::sendInLoop(string&& message){
    loop_.lock()->assertInLoopThread();
    ssize_t cnt = 0;
    if(!channel_->isWriting() && outputBuffer_.readableLength() == 0){
        cnt = ::write(channel_->fd(), message.data(), message.size());
        if(cnt >= 0){
            if(static_cast<size_t>(cnt)<message.size()){
                // 
            }
        }else{
            cnt =0;
            if(errno!=EWOULDBLOCK){
                perror("write");
                cerr<<"TcpConnection::sendInLoop()"<<endl;
            }
        }
    }
    if(static_cast<size_t>(cnt)<message.size())
    {
        outputBuffer_.append(message.data() + cnt, message.size() - cnt);
        if(!channel_->isWriting()){
            channel_->enableWriting();
        }
    }
}