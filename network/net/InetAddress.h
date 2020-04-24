#ifndef _INETADDRESS_H_
#define _INETADDRESS_H_

#include <arpa/inet.h>
#include <string>
#include "network_global.h"

class InetAddress {
    using string = std::string;

public:
    explicit InetAddress(const sockaddr_in6& addr6) : sockaddr6_{addr6} {}
    explicit InetAddress(const sockaddr_in& addr) : sockaddr_{addr} {}
    InetAddress(const string& ip, const uint16_t port, bool ipv6 = false);
    InetAddress(const uint16_t port);
    string toIp()const;
    string toIpPort()const;
    uint16_t toPort()const;
    const sockaddr* getSockAddr()const {return sockets::sockaddr_cast<sockaddr*>(&sockaddr6_);}
    void setSockAddr(sockaddr* addr);
    uint16_t getNetPort()const {return sockaddr6_.sin6_port;}

private:
    union {
        sockaddr_in sockaddr_;
        sockaddr_in6 sockaddr6_;
    };
};

#endif