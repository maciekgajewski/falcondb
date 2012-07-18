project(FalconDB)
cmake_minimum_required(VERSION 2.8)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmakeModules)

find_package(Boost REQUIRED)
find_package(LevelDB)
find_package(NessDB) 

# project-wide setup
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++0x -Wall")
include_directories(${CMAKE_SOURCE_DIR})
set(CMAKE_BUILD_TYPE Debug)

#####################
# project components
#####################
if(NESSDB_FOUND)
option(COMPILE_NESSDB_BACKEND "" ON)
endif()
if(LEVELDB_FOUND)
option(COMPILE_LEVELDB_BACKEND "" ON)
endif()

# db core
add_subdirectory(dbengine)
add_subdirectory(interfaces)

# storage backends
if(COMPILE_NESSDB_BACKEND)
add_subdirectory(backend_nessdb)
endif()
if(COMPILE_LEVELDB_BACKEND)
add_subdirectory(backend_leveldb)
endif()

# frontends
add_subdirectory(console_frontend)

# apps
add_subdirectory(ifalcon)

# general-purpose utilities
add_subdirectory(bson)
add_subdirectory(utils)
add_subdirectory(threads)

