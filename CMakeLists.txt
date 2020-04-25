cmake_minimum_required(VERSION 2.8)

ADD_COMPILE_OPTIONS("-std=c++11" "-pthread" "-g" "-Wall")
project(network)

add_subdirectory(./network/net)
add_subdirectory(./network/base)

add_subdirectory(s01)
add_subdirectory(s02)
add_subdirectory(s03)
add_subdirectory(s04)
add_subdirectory(s05)
add_subdirectory(s06)
add_subdirectory(s07)
add_subdirectory(s08)
add_subdirectory(s09)
add_subdirectory(s10)

