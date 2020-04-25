
add_library(s10_network EventLoop.cpp 
Poller.cpp Channel.cpp Channel.h Timer.cpp 
TimerQueue.cpp EventLoopThread.cpp
Acceptor.cpp Socket.cpp InetAddress.cpp
TcpServer.cpp TcpConnection.cpp RingBuffer.cpp
EventLoopThreadPool.cpp
)

target_link_libraries(s10_network s10_base)