
#include "Connector.h"
#include "../base/SocketTools.h"
#include <cassert>
#include "Channel.h"
Connector::Connector(std::weak_ptr<EventLoop> loop,
                     const InetAddress& serverAddr)
    : loop_{loop},
      serverAddr_{serverAddr},
      connect_{false},
      sockfd_{sockets::createNonblockingSocket(AF_INET)},
      state_{State::disconnected}
       {}


Connector::~Connector()
{
    
}

void Connector::start()
{
    assert(loop_.lock());
    connect_ = true;
    loop_.lock()->runInLoop(std::bind(&Connector::startInLoop, this));
}

void Connector::restart()
{
    assert(loop_.lock());
    loop_.lock()->assertInLoopThread();
    setState(State::disconnected);
    retryDelay_ms_ = kInitRetryDelay_ms;
    connect_ = true;
    startInLoop();
}

void Connector::stop()
{
    connect_ = false;
    loop_.lock()->cancel(timerid_);
}


void Connector::connect()
{
    int sockfd = sockets::createNonblockingSocket(AF_INET);
    int ret = sockets::connect(sockfd, serverAddr_.getSockAddr());
    int savedErrno = (ret == 0) ? 0 : errno;
    switch (savedErrno)
    {
    case 0:
    case EINPROGRESS:
    case EINTR:
    case EISCONN:
        break;
    
    case EAGAIN:
    case EADDRINUSE:
    case EADDRNOTAVAIL:
    case ECONNREFUSED:
    case ENETUNREACH:
        break;
    
    case EACCES:
    case EPERM:
    case EAFNOSUPPORT:
    case EALREADY:
    case EBADF:
    case EFAULT:
    case ENOTSOCK:

        break;
    
    default:
        break;
    }
}


void Connector::Connecting(int sockfd)
{
    setState(State::connecting);
    assert(!channel_);
    channel_.reset(new Channel(loop_, sockfd));
    channel_->setReadCallback(&Connector::handleRead,this);
    channel_->setWriteCallback(&Connector::handleWrite,this);
    channel_->setCloseCallback(&Connector::handleClose,this);
    channel_->enableWriting();
}

int Connector::removeAndResetChannel()
{
    
}

void Connector::resetChannel()
{
    channel.reset();
}

void Connector::handleWrite()
{

}

void Connector::handleError()
{
    Critical()<<"Connector::handleError";
    int sockfd = removeAndResetChannel();
    int err = sockets::getSocketError(sockfd);
    
}


