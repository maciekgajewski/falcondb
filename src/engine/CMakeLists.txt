add_library(engine
    engine.cpp engine.hpp
)

target_link_libraries(engine
    boost_filesystem
)
