cmake_minimum_required(VERSION 2.8
        )
project(judge_core)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
        main.cpp
#        sources/redis_client.cpp
        sources/judge.cpp
        sources/daemon.cpp
        sources/utils.cpp)

include_directories(includes)
#link_libraries(/usr/local/lib/libcpp_redis.so)
link_libraries(/usr/lib/x86_64-linux-gnu/libmysqlclient.so)
add_executable(judged ${SOURCE_FILES})