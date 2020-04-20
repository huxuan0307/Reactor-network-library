#ifndef _CALLBACK_H_
#define _CALLBACK_H_
#include <functional>
#include "InetAddress.h"
using std::function;
using TimerCallback = function<void()>;
using Event = std::function<void()>;
using NewConnectionCallback_t = std::function<void(int sockfd, const InetAddress &)>;
#endif