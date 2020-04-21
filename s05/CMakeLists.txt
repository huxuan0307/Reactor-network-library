
find_package (Threads)

add_library(s05_network EventLoop.cpp EventLoop.h 
Poller.cpp Poller.h Channel.cpp Channel.h Timer.cpp 
TimerQueue.cpp Timestamp.cpp EventLoopThread.cpp
Acceptor.cpp SocketTools.cpp Socket.cpp InetAddress.cpp
TcpServer.cpp TcpConnection.cpp
)

add_executable(s05_test8 test8.cpp)
target_link_libraries(s05_test8 s05_network ${CMAKE_THREAD_LIBS_INIT})
