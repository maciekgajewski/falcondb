project(FalconDB)
cmake_minimum_required(VERSION 2.8)

# project-wide setup
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++0x -Wall")
include_directories(${CMAKE_SOURCE_DIR})
set(CMAKE_BUILD_TYPE Debug)

#####################
# project components
#####################

# db core
add_subdirectory(dbengine)
add_subdirectory(interfaces)
add_subdirectory(document)

# storage backends
add_subdirectory(backend_nessdb)
add_subdirectory(backend_leveldb)

# frontends
add_subdirectory(console_frontend)

# indexes
add_subdirecotry(indexes)

# apps
add_subdirectory(ifalcon)

# general-purpose utilities
add_subdirectory(bson)
add_subdirectory(bson_test)
add_subdirectory(utils)
add_subdirectory(threads)

