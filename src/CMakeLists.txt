cmake_minimum_required(VERSION 3.5)
project(rideos_oa)
set(CMAKE_CXX_STANDARD 14)

add_executable(rideos_oa main.cpp
               robot_routing_env.h robot_routing_env.cpp
               planner.h planner.cpp)
include_directories(../robot_routing)
