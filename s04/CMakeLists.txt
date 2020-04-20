
find_package (Threads)

add_executable(s04_test7 test7.cpp)
target_link_libraries(s04_test7 Network ${CMAKE_THREAD_LIBS_INIT})

add_executable(s04_test8 test8.cpp)
target_link_libraries(s04_test8 Network ${CMAKE_THREAD_LIBS_INIT})

add_executable(s04_test9 test9.cpp)
target_link_libraries(s04_test9 Network ${CMAKE_THREAD_LIBS_INIT})

