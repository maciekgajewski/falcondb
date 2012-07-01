project(FalconDB)
cmake_minimum_required(VERSION 2.8)

# project-wide setup
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++0x")
include_directories(${CMAKE_SOURCE_DIR})

# project components

add_subdirectory(console_frontend)
add_subdirectory(ifalcon)
add_subdirectory(interfaces)
add_subdirectory(bson)
add_subdirectory(engine)

