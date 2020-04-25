add_subdirectory(base)
add_subdirectory(net)

add_executable(s09_test10 test10.cpp)
target_link_libraries(s09_test10 s09_network)
add_executable(s09_test11 test11.cpp)
target_link_libraries(s09_test11 s09_network)