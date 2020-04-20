
add_library(Network EventLoop.cpp EventLoop.h 
Poller.cpp Poller.h Channel.cpp Channel.h Timer.cpp 
TimerQueue.cpp Timestamp.cpp EventLoopThread.cpp
Acceptor.cpp SocketTools.cpp Socket.cpp InetAddress.cpp
)
