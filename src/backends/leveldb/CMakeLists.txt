include_directories(${LEVELDB_INCLUDE_DIR})

add_library(backend_leveldb
    backend.cpp backend.hpp
    database.cpp database.hpp
)

target_link_libraries(backend_leveldb
    ${LEVELDB_LIBRARY}
    ${SNAPPY_LIBRARY}
    utils
)
