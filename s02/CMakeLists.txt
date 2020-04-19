find_package (Threads)
add_library(s02_network EventLoop.cpp EventLoop.h Poller.cpp Poller.h Channel.cpp Channel.h Timer.cpp TimerQueue.cpp Timestamp.cpp)
add_executable(test4 test4.cpp)
target_link_libraries(test4 s02_network ${CMAKE_THREAD_LIBS_INIT})


add_executable(timestamp_test timestamp_test.cpp)
target_link_libraries(timestamp_test s02_network ${CMAKE_THREAD_LIBS_INIT})