cmake_minimum_required(VERSION 3.16)
project(SlimeFinder C)

set(CMAKE_C_STANDARD 99)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(SlimeFinder main.c)
target_link_libraries(SlimeFinder Threads::Threads)
target_link_libraries(SlimeFinder m)