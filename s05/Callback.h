#ifndef _CALLBACK_H_
#define _CALLBACK_H_
#include <functional>
#include <memory>
// #include "InetAddress.h"
class InetAddress;
class TcpConnection;

using std::function;
using TimerCallback = function<void()>;
using Event = std::function<void()>;
using NewConnectionCallback_t = std::function<void(int sockfd, const InetAddress &)>;
using ConnectionCallback_t = std::function<void(const std::shared_ptr<TcpConnection>&)>;
using MessageCallback_t = std::function<void(const std::shared_ptr<TcpConnection> &, const char *, ssize_t len)>;
#endif