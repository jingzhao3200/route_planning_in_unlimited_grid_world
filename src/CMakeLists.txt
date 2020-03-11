cmake_minimum_required(VERSION 3.15)
project(rideos_oa)

set(CMAKE_CXX_STANDARD 14)

add_executable(rideos_oa main.cpp record_laser_path.cpp)
include_directories(../robot_routing)