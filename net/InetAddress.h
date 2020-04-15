#include <arpa/inet.h>
#include <string>
#include "SocketTools.h"
class InetAddress {
    using string = std::string;

public:
    explicit InetAddress(const sockaddr_in6& addr6) : sockaddr6_{addr6} {}
    explicit InetAddress(const sockaddr_in& addr) : sockaddr_{addr} {}
    InetAddress(const string& ip, const uint16_t port, bool ipv6 = false);
    string toIp()const;
    string toIpPort()const;
    uint16_t toPort()const;
    const sockaddr* getSockAddr()const {return sockets::sockaddr_cast<sockaddr*>(&sockaddr6_);}

private:
    union {
        sockaddr_in sockaddr_;
        sockaddr_in6 sockaddr6_;
    };
};