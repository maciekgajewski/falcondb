include_directories(${Boost_INCLUDE_DIR})
add_library(utils
    range.hpp
    string.hpp
    exception.cpp exception.hpp
    tscclock.hpp
    rwmutex.cpp rwmutex.hpp
    filesystem.hpp
)
