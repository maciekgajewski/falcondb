add_library(backend_leveldb
    backend.cpp backend.hpp
    database.cpp database.hpp
)

target_link_libraries(backend_leveldb

    leveldb
    utils
)
