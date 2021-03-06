project(FalconDB)
cmake_minimum_required(VERSION 2.8)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmakeModules)

find_package(Boost REQUIRED system thread unit_test_framework filesystem date_time)
find_package(LevelDB)
find_package(NessDB) 

include_directories(${Boost_INCLUDE_DIR})
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L${Boost_LIBRARY_DIRS}")

message(STATUS "Boost include dir ${Boost_INCLUDE_DIR}")
message(STATUS "Boost library dir ${Boost_LIBRARY_DIRS}")
message(STATUS "Boost library ${Boost_LIBRARIES}")

# project-wide setup
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++0x -Wall")
include_directories(${CMAKE_SOURCE_DIR})

message(STATUS "CXX_FLAGS ${CMAKE_CXX_FLAGS}")

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
add_subdirectory(document)
add_subdirectory(document_test)

# storage backends
if(COMPILE_NESSDB_BACKEND)
add_subdirectory(backend_nessdb)
endif()
if(COMPILE_LEVELDB_BACKEND)
add_subdirectory(backend_leveldb)
endif()

# frontends
add_subdirectory(console_frontend)
add_subdirectory(frontend)

# indexes
add_subdirectory(indexes)

# apps
add_subdirectory(ifalcon)
add_subdirectory(mongofalcon)

# general-purpose utilities
add_subdirectory(bson)
add_subdirectory(bson_test)
add_subdirectory(utils)
add_subdirectory(threads)
