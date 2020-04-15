#include "Socket.h"
#include "SocketTools.h"
#include "InetAddress.h"
Socket::~Socket(){
    sockets::close(sockfd_);
}

void Socket::bindAddress(const InetAddress& addr){
    sockets::bindOrDie(sockfd_, addr.getSockAddr());
}