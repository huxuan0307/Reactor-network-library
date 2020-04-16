
find_package (Threads)
add_executable(test1 test1.cpp)
add_executable(test2 test2.cpp)
target_link_libraries(test1 EventLoop ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(test2 EventLoop ${CMAKE_THREAD_LIBS_INIT})
