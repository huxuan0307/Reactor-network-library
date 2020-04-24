
add_library(s08_network EventLoop.cpp EventLoop.h 
Poller.cpp Poller.h Channel.cpp Channel.h Timer.cpp 
TimerQueue.cpp Timestamp.cpp EventLoopThread.cpp
Acceptor.cpp SocketTools.cpp Socket.cpp InetAddress.cpp
TcpServer.cpp TcpConnection.cpp RingBuffer.cpp
)


add_executable(s08_test10 test10.cpp)
target_link_libraries(s08_test10 s08_network )