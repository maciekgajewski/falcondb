add_library(backend_nessdb
    backend.cpp backend.hpp
    database.cpp database.hpp
)

target_link_libraries(backend_nessdb

    nessdb
)
