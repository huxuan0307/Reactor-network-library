find_package (Threads)

add_subdirectory(base)
add_subdirectory(net)

add_executable(s10_test8 test8.cpp)
target_link_libraries(s10_test8 s10_network ${CMAKE_THREAD_LIBS_INIT})

add_executable(s10_test9 test9.cpp)
target_link_libraries(s10_test9 s10_network ${CMAKE_THREAD_LIBS_INIT})

add_executable(s10_test10 test10.cpp)
target_link_libraries(s10_test10 s10_network ${CMAKE_THREAD_LIBS_INIT})

add_executable(s10_test11 test11.cpp)
target_link_libraries(s10_test11 s10_network ${CMAKE_THREAD_LIBS_INIT})
