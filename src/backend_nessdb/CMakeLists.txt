include_directories(${NESSDB_INCLUDE_DIR})
add_library(backend_nessdb
    backend.cpp backend.hpp
    database.cpp database.hpp
)

target_link_libraries(backend_nessdb
    ${NESSDB_LIBRARY}
    utils
)
