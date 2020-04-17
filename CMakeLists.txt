cmake_minimum_required(VERSION 2.8)

ADD_COMPILE_OPTIONS("-std=c++11" "-pthread")
project(network)

add_subdirectory(./network/net)
add_subdirectory(s01)
