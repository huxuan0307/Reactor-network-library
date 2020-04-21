cmake_minimum_required(VERSION 2.8)

ADD_COMPILE_OPTIONS("-std=c++11" "-pthread" "-g")
project(network)

add_subdirectory(./network/net)
add_subdirectory(s01)
add_subdirectory(s02)
add_subdirectory(s03)
add_subdirectory(s04)
add_subdirectory(s05)
add_subdirectory(s06)

