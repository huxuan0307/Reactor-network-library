
#include "TcpConnection.h"

#include <unistd.h>
#include <cassert>
#include "Channel.h"
#include "Socket.h"
#include <iostream>
#include <cassert>
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
      name_{connName}
{
    cout<<"TcpConnection::TcpConnection()... "<<endl;
    channel_->setReadCallback(std::bind(&TcpConnection::handleRead, this));
    cout<<"TcpConnection::TcpConnection() end "<<endl;
}

TcpConnection::~TcpConnection() {
    cout<<"????"<<endl;
}

void TcpConnection::handleRead() {
    cout<<"TcpConnection::handleRead()"<<endl;
    char buf[65536];
    ssize_t cnt = ::read(channel_->fd(), buf, sizeof buf);
    messageCallback_(shared_from_this(), buf, sizeof cnt);
}

void TcpConnection::connectEstablished()
{
    cout<<"TcpConnection::connectEstablished()"<<endl;
    assert(loop_.lock());
    loop_.lock()->assertInLoopThread();
    assert(state_==State::connecting);
    setState(State::connected);
    channel_->enableReading();
    connectionCallback_(shared_from_this());
}

