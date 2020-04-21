
find_package (Threads)

# add_library()

add_executable(s06_test8 test8.cpp)
target_link_libraries(s06_test8 Network ${CMAKE_THREAD_LIBD_INIT})