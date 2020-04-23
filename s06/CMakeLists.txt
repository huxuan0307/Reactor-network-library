
find_package (Threads)

add_library(s06_network EventLoop.cpp EventLoop.h 
Poller.cpp Poller.h Channel.cpp Channel.h Timer.cpp 
TimerQueue.cpp Timestamp.cpp EventLoopThread.cpp
Acceptor.cpp SocketTools.cpp Socket.cpp InetAddress.cpp
TcpServer.cpp TcpConnection.cpp
)

add_executable(s06_test8 test8.cpp)
target_link_libraries(s06_test8 s06_network ${CMAKE_THREAD_LIBD_INIT})