#include "noncopyable.h"

class InetAddress;

class Socket
{
    NONCOPYABLE(Socket)
public:
    explicit Socket(int sockfd)
        : sockfd_{sockfd}
    {
    }
    ~Socket();
    int fd() const {return sockfd_;}
    bool getTcpInfo(struct tcp_info *) const;
    // abort if listen error
    void bindAddress(const InetAddress& addr);
    // abort if listen error
    void listen();

    // if success, return fd of the accepted socket
    // peeraddr contains info of peer client
    // if failed, return -1, peeraddr is untouched
    int accept(InetAddress* peeraddr);
    // 
    void shutdownWrite();

    void setReuseAddr(bool on = true);
    void setReusePort(bool on = true);
    void setTcpNoDelay(bool on = true);
private:
    // 无法更改的socket 句柄
    const int sockfd_;
};