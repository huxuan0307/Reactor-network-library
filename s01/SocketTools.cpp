#include <SocketTools.h>
#include <unistd.h>
#include <type_traits>
#include <cstdio>
#include <iostream>
#include <chrono>
#include <cstring>
using std::cerr;
using std::endl;
using namespace std;
namespace sockets {
int createNonblockingSocket(sa_family_t family, bool close_on_exec) {
    int sockfd;
    if (close_on_exec)
        sockfd = ::socket(family, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC,
                          IPPROTO_TCP);
    else
        sockfd = ::socket(family, SOCK_STREAM | SOCK_NONBLOCK, IPPROTO_TCP);

    if (sockfd < 0) {
        // error
        cerr << "sockets::createNonblockingSocket\n";
    }
    return sockfd;
}

void shutdownWrite(int sockfd) {
    if (::shutdown(sockfd, SHUT_WR) < 0) {
        cerr << "sockets::shutdownWrite\n";
    }
}

void close(int sockfd) {
    if (::close(sockfd) < 0) cerr << "sockets::close\n";
}

int bind(int sockfd, const sockaddr* addr) {
    int ret = -1;
    if(addr->sa_family==AF_INET6){
        ret = ::bind(sockfd, addr, static_cast<socklen_t>(sizeof(sockaddr_in6)));
        if(ret<0){
            cerr << "sockets::bind\n";
        }
    }else if(addr->sa_family==AF_INET){
        ret = ::bind(sockfd, addr, static_cast<socklen_t>(sizeof(sockaddr_in)));
        if(ret<0){
            cerr << "sockets::bind\n";
        }
    }
    return ret;
}

void bindOrDie(int sockfd, const sockaddr* addr) {
    if(addr->sa_family==AF_INET6){
        int ret = ::bind(sockfd, addr, static_cast<socklen_t>(sizeof(sockaddr_in6)));
        if(ret<0){
            cerr << "sockets::bind\n";
            abort();
        }
    }else if(addr->sa_family==AF_INET){
        int ret = ::bind(sockfd, addr, static_cast<socklen_t>(sizeof(sockaddr_in)));
        if(ret<0){
            cerr << "sockets::bind\n";
            abort();
        }
    }

}

int listen(int sockfd) {
    int ret = ::listen(sockfd, SOMAXCONN);
    if (ret < 0) {
        cerr<<"socket::listen\n";
    }
    return ret;
}

void listenOrDie(int sockfd) {
    int ret = ::listen(sockfd, SOMAXCONN);
    if (ret < 0) {
        cerr<<"socket::listen\n";
        abort();
    }
}


int accept(int sockfd, sockaddr_in* addr) {
    socklen_t addrlen = static_cast<socklen_t>(sizeof(sockaddr_in));
#if defined (NO_ACCEPT4)
    int sockclntfd = ::accept(sockfd, sock, sockaddr_cast<sockaddr*>(addr), &addrlen);
    // non-block
    int flags = ::fcntl(sockclntfd, F_GETFL, 0);
    flags |= O_NONBLOCK;
    int ret = ::fcntl(sockclntfd, F_SETFL, flags);
    // FIXME check

    // close-on-exec
    flags = ::fcntl(sockclntfd, F_GETFD, 0);
    flags |= FD_CLOEXEC;
    ret = ::fcntl(sockclntfd, F_SETFD, flags);
#else
    int sockclntfd = ::accept4(sockfd, sockaddr_cast<sockaddr*>(addr), &addrlen, SOCK_NONBLOCK | SOCK_CLOEXEC);
#endif
    if(sockclntfd < 0){
        int savedErrno = errno;
        cerr<<"Sockets::accept\n";
        switch (savedErrno)
        {
        case EAGAIN:
        case ECONNABORTED:  // A connection has been aborted.
        case EFAULT:        // The addr argument is not in a writeable part of the user address space.
        case EINTR:         // syscall was interrupted by a signal that was caught before a valid connection arrived
        case EMFILE:        // The per-process limit on number of open file descriptors has been reached.
        case ENFILE:        // The system-wide limit on total number of open files has been reached.
        case EPERM:         // accept only, Linux Firewall rules forbid connection.
            // expected errors
            break;
        case EINVAL:        // Socket is not listening for connections, or addrlen is invalid(e.g. is negative)
                            // accept4: invalid value in flags
        case ENOBUFS:
        case ENOMEM:        // Not enough free memory. often means that the memory allocation is limited by the socket buffer limits.
                            // not by the system memory
        case EBADF:         // sockfd is not an open file descriptor
        case ENOTSOCK:      // The file descriptor sockdf does not refer to a socket?
        case EOPNOTSUPP:    // The referenced socket is not of type SOCK_STREAM
        case EPROTO:        // Protocal error
        case ENOSR:
        case ESOCKTNOSUPPORT:
        case EPROTONOSUPPORT:
        case ETIMEDOUT:
            // unexpected errors
            perror("accept");
            abort();
            break;
        
        default:
            cerr<<"unknown error of accept"<<savedErrno<<endl;
            break;
        }

    }
    return sockclntfd;
}

int connect(int sockfd, const sockaddr* addr){
    int ret = ::connect(sockfd, addr, static_cast<socklen_t>(sizeof(*addr)));
    if(ret<0){
        cerr<<"sockets::connect"<<endl;
    }
    return ret;
}

std::string toIp(const sockaddr* addr){
    char ip[64];
    if(addr->sa_family == AF_INET6){
        auto addr6 = sockaddr_cast<sockaddr_in6*>(addr);
        inet_ntop(AF_INET6, &addr6->sin6_addr, ip, static_cast<socklen_t>(sizeof (sockaddr_in6)));
    }else if(addr->sa_family == AF_INET){
        auto addr4 = sockaddr_cast<sockaddr_in*>(addr);
        inet_ntop(AF_INET, &addr4->sin_addr, ip, static_cast<socklen_t>(sizeof (sockaddr_in)));
    }
    return ip;
}

std::string toIpPort(const sockaddr* addr){
    char ipport[64]{'\0'};
    if(addr->sa_family == AF_INET6){
        auto addr6 = sockaddr_cast<sockaddr_in6*>(addr);
        inet_ntop(AF_INET6, &addr6->sin6_addr, ipport, static_cast<socklen_t>(sizeof (sockaddr_in6)));
        auto p = ipport+strlen(ipport);
        *p++ = ':';
        sprintf(p, "%u", sockets::netToHost16(addr6->sin6_port));
    }else if(addr->sa_family == AF_INET){
        auto addr4 = sockaddr_cast<sockaddr_in*>(addr);
        inet_ntop(AF_INET, &addr4->sin_addr, ipport, static_cast<socklen_t>(sizeof (sockaddr_in)));
        auto p = ipport+strlen(ipport);
        *p++ = ':';
        sprintf(p, "%u", sockets::netToHost16(addr4->sin_port));
    }
    return ipport;
}

std::uint16_t toPort(const sockaddr* addr){
    if(addr->sa_family == AF_INET6){
        return sockets::netToHost16(sockaddr_cast<sockaddr_in6*>(addr)->sin6_port);
    }else if(addr->sa_family == AF_INET){
        return sockets::netToHost16(sockaddr_cast<sockaddr_in*>(addr)->sin_port);
    }
}

}  // namespace sockets