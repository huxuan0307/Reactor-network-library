
find_package (Threads)
add_library(s01_network EventLoop.cpp EventLoop.h Poller.cpp Poller.h Channel.cpp Channel.h)
add_executable(test3 test3.cpp)
target_link_libraries(test3 s01_network ${CMAKE_THREAD_LIBS_INIT})
