
add_library(s09_network EventLoop.cpp 
Poller.cpp Channel.cpp Channel.h Timer.cpp 
TimerQueue.cpp EventLoopThread.cpp
Acceptor.cpp Socket.cpp InetAddress.cpp
TcpServer.cpp TcpConnection.cpp RingBuffer.cpp
)

target_link_libraries(s09_network s09_base)