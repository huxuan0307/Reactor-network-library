#include "InetAddress.h"
#include <string.h>
using std::string;

InetAddress::InetAddress(const string& ip, const uint16_t port, bool ipv6 = false){
    bzero(&sockaddr6_, sizeof(sockaddr_in6));
    if(ipv6){
        sockaddr6_.sin6_family = AF_INET6;
        sockaddr6_.sin6_port = sockets::hostToNet16(port);
        inet_pton(AF_INET6, ip.data(), &sockaddr6_.sin6_addr);
    }else{
        sockaddr_.sin_family = AF_INET;
        sockaddr_.sin_port = sockets::hostToNet16(port);
        inet_pton(AF_INET, ip.data(), &sockaddr_.sin_addr);
    }
}

string InetAddress::toIp()const {
    return sockets::toIp(getSockAddr());
}

string InetAddress::toIpPort()const{
    return sockets::toIpPort(getSockAddr());
}

uint16_t InetAddress::toPort()const{
    return sockets::toPort(getSockAddr());
}

void InetAddress::setSockAddr(sockaddr* addr){
    if(addr->sa_family == AF_INET6){
        memcpy(&sockaddr6_, addr, sizeof(sockaddr_in6));
    }
    else if(addr->sa_family == AF_INET){
        memcpy(&sockaddr_, addr, sizeof(sockaddr_in));
    }
}
