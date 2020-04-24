
add_library(s07_network EventLoop.cpp EventLoop.h 
Poller.cpp Poller.h Channel.cpp Channel.h Timer.cpp 
TimerQueue.cpp Timestamp.cpp EventLoopThread.cpp
Acceptor.cpp SocketTools.cpp Socket.cpp InetAddress.cpp
TcpServer.cpp TcpConnection.cpp RingBuffer.cpp
)

add_executable(s07_test3 test3.cpp)
target_link_libraries(s07_test3 s07_network)
add_executable(s07_test8 test8.cpp)
target_link_libraries(s07_test8 s07_network)
