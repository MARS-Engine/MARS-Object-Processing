cmake_minimum_required(VERSION 3.26)
project(MARS-Object-Processing)

find_package(TBB REQUIRED)

set(CMAKE_CXX_STANDARD 23)

add_executable(MARS-Object-Processing main.cpp)
target_link_libraries(MARS-Object-Processing PUBLIC TBB::tbb)