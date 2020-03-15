cmake_minimum_required(VERSION 3.5)
project(route_plan)
set(CMAKE_CXX_STANDARD 14)

add_executable(route_plan main.cpp
               robot_routing_env.h robot_routing_env.cpp
               planner.h planner.cpp)
include_directories(../robot_routing)
