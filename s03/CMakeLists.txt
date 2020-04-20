find_package (Threads)

add_library(s03_network EventLoop.cpp EventLoop.h 
Poller.cpp Poller.h Channel.cpp Channel.h Timer.cpp 
TimerQueue.cpp Timestamp.cpp EventLoopThread.cpp)

add_executable(s03_test4 test4.cpp)
target_link_libraries(s03_test4 s03_network)

add_executable(s03_test5 test5.cpp)
target_link_libraries(s03_test5 s03_network)

add_executable(s03_test6 test6.cpp)
target_link_libraries(s03_test6 s03_network ${CMAKE_THREAD_LIBS_INIT})
