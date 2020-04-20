#include "Socket.h"
#include "SocketTools.h"
#include "InetAddress.h"
#include <netinet/tcp.h>
#include <iostream>
using namespace std;

Socket::~Socket(){
    sockets::close(sockfd_);
}

void Socket::bindAddress(const InetAddress& addr){
    sockets::bindOrDie(sockfd_, addr.getSockAddr());
}

void Socket::listen(){
    sockets::listenOrDie(sockfd_);
}

int Socket::accept(InetAddress* peeraddr){
    sockaddr_in6 addr6;
    cout<<"accept!!"<<endl;
    int connfd = sockets::accept(sockfd_,&addr6);
    if(connfd>=0){
        peeraddr->setSockAddr(sockets::sockaddr_cast<sockaddr*>(&addr6));
    }
    return connfd;
}

void Socket::shutdownWrite(){
    sockets::shutdownWrite(sockfd_);
}

void Socket::setReuseAddr(bool on){
    int optval = on;
    ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);
}

void Socket::setReusePort(bool on){
    int optval = on;
    ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof optval);
}

void Socket::setTcpNoDelay(bool on){
    int optval = on;
    ::setsockopt(sockfd_, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof optval);
}

