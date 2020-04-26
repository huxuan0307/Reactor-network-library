#include "TcpConnector.h"
#include "Channel.h"
using namespace std;

TcpConnector::TcpConnector(std::weak_ptr<EventLoop> loop, const InetAddress& serverAddr)
:loop_{loop}, serverAddr_{serverAddr}, connect_{false}, state_{State::disconnected}, retryDelay_ms_{kInitRetryDelay_ms}
{
    assert(loop_.lock());

}

TcpConnector::~TcpConnector()
{
    loop_.lock()->cancelTimer(timerid_);
    assert(!channel_);
}

void TcpConnector::start()
{
    connect_ = true;
    loop_.lock()->runInLoop(std::bind(&TcpConnector::startInLoop, this));
}

void TcpConnector::startInLoop()
{
    loop_.lock()->assertInLoopThread();
    assert(state_==State::disconnected);
    if(connect_){
        connect();
    }else{
        cerr<<"???do not connect?"<<endl;
    }
}

void TcpConnector::connect()
{
    int sockfd =sockets::createNonblockingSocket(AF_INET);
    int ret = sockets::connect(sockfd, serverAddr_.getSockAddr());
    int savedErrno = (ret == 0)?0:errno;
    switch  (savedErrno)
    {
    case 0:
    case EINPROGRESS:
    case EINTR:
    case EISCONN:
        connecting(sockfd);
        break;
    case EAGAIN:
    case EADDRINUSE:
    case EADDRNOTAVAIL:
    case ECONNREFUSED:
    case ENETUNREACH:
        retry(sockfd);
        break;
    case EACCES:
    case EPERM:
    case EAFNOSUPPORT:
    case EALREADY:
    case EBADF:
    case EFAULT:
    case ENOTSOCK:
        FATAL<<"TcpConnector::connect() connect error "<<strerror(savedErrno);
        sockets::close(sockfd);
        break;
    default:
        FATAL<<"TcpConnector::connect() unexpected error ";
        sockets::close(sockfd);
        break;
    }
}

void TcpConnector::restart()
{
    loop_.lock()->assertInLoopThread();
    setState(State::disconnected);
    retryDelay_ms_ = kInitRetryDelay_ms;
    connect_ = true;
    startInLoop();
}

void TcpConnector::stop(){
    connect_ = false;
    loop_.lock()->cancelTimer(timerid_);
}

void TcpConnector::connecting(int sockfd)
{
    setState(State::connecting);
    assert(!channel_);
    channel_.reset(new Channel{loop_, sockfd});
    channel_->setWriteCallback(std::bind(&TcpConnector::handleWrite,this));
    channel_->setReadCallback(std::bind(&TcpConnector::handleRead, this));
    
    channel_->enableReading();
}

void TcpConnector::removeChannel()
{
    channel_->disableAll();
    loop_.lock()->removeChannel(channel_);
}

void TcpConnector::resetChannel()
{
    channel_.reset();
}

int TcpConnector::removeAndResetChannel()
{
    removeChannel();
    int sockfd = channel_->fd();    // why this fd instead of a new one?
    loop_.lock()->queueInLoop(std::bind(&TcpConnector::resetChannel, this));
    return sockfd;
}



void TcpConnector::handleWrite()
{
    TRACE << "TcpConnector::handleWrite()";
    if(state_ == State::connecting){
        int sockfd = removeAndResetChannel();
        int err = sockets::getSocketError(sockfd);
        if(err){
            WARNING << "TcpConnector::handleWrite() - SO_ERROR = " << err << " "
                    << strerror(err);
            retry(sockfd);
        }
        else if(sockets::isSelfConnect(sockfd)){
            WARNING<<"TcpConnector::handleWrite() - Self connect";
            retry(sockfd);
        }
        else{
            setState(State::connected);
            if(connect_){
                newConnectionCallback_(sockfd);
            }
            else{
                sockets::close(sockfd);
            }
        }
    }else{
        assert(state_ == State::disconnected);
    }
}

void TcpConnector::handleError()
{
    CRITICAL<<"Connector::handleError()";
    assert(state_ == State::connecting);
    int sockfd = removeAndResetChannel();
    int err = sockets::getSocketError(sockfd);
    TRACE<<"SO_ERROR = "<<err<<" "<<strerror(err);
    retry(sockfd);
}

void TcpConnector::retry(int sockfd)
{
    sockets::close(sockfd);
    setState(State::disconnected);
    if(connect_){
        Info()<<"Connector::retry - Retry connecting to "<<serverAddr_.toIpPort()<<" in "<<retryDelay_ms_<<" ms. ";
        timerid_ = loop_.lock()->run_after(retryDelay_ms_/1000.0, std::bind(&TcpConnector::startInLoop, this));
        retryDelay_ms_ = std::min(retryDelay_ms_ * 2, kMaxRetryDelay_ms);
    }else{
        TRACE<<"do not connect";
    }
}
